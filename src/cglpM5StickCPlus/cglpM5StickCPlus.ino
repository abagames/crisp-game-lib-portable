#define LGFX_AUTODETECT
#define LGFX_USE_V1

#include <LGFX_AUTODETECT.hpp>
#include <LovyanGFX.hpp>

#define BUTTON_A_PIN 37
#define BUTTON_B_PIN 39

#define _M5DISPLAY_H_
class M5Display {};
#include <M5StickCPlus.h>
#include <float.h>

#include "cglp.h"
#include "machineDependent.h"

static LGFX lcd;
static LGFX_Sprite canvas(&lcd);
static bool isCanvasCreated = false;
static int canvasX;
static int canvasY;

typedef struct {
  float freq;
  float duration;
  float when;
} SoundTone;

#define TONE_PER_NOTE 32
#define SOUND_TONE_COUNT 64
static SoundTone soundTones[SOUND_TONE_COUNT];
static int soundToneIndex = 0;
static float soundTime = 0;

static void initSoundTones() {
  for (int i = 0; i < SOUND_TONE_COUNT; i++) {
    soundTones[i].when = FLT_MAX;
  }
}

static void addSoundTone(float freq, float duration, float when) {
  SoundTone *st = &soundTones[soundToneIndex];
  st->freq = freq;
  st->duration = duration;
  st->when = when;
  soundToneIndex++;
  if (soundToneIndex >= SOUND_TONE_COUNT) {
    soundToneIndex = 0;
  }
}

void md_drawRect(float x, float y, float w, float h, unsigned char r,
                 unsigned char g, unsigned char b) {
  canvas.fillRect((int)x, (int)y, (int)w, (int)h, lcd.color565(r, g, b));
}

#define TRANSPARENT_COLOR 0

typedef struct {
  LGFX_Sprite *sprite;
  int hash;
} CharaterSprite;

static CharaterSprite characterSprites[MAX_CACHED_CHARACTER_PATTERN_COUNT];
static int characterSpritesCount;

static void initCharacterSprite() { characterSpritesCount = 0; }

static uint16_t characterImageData[CHARACTER_WIDTH * CHARACTER_HEIGHT];

static void createCharacterImageData(
    unsigned char grid[CHARACTER_HEIGHT][CHARACTER_WIDTH][3]) {
  int cp = 0;
  for (int y = 0; y < CHARACTER_HEIGHT; y++) {
    for (int x = 0; x < CHARACTER_WIDTH; x++) {
      unsigned char r = grid[y][x][0];
      unsigned char g = grid[y][x][1];
      unsigned char b = grid[y][x][2];
      characterImageData[cp] =
          (r > 0 || g > 0 || b > 0) ? lcd.color565(r, g, b) : TRANSPARENT_COLOR;
      cp++;
    }
  }
}

void md_drawCharacter(unsigned char grid[CHARACTER_HEIGHT][CHARACTER_WIDTH][3],
                      float x, float y, int hash) {
  CharaterSprite *cp = NULL;
  for (int i = 0; i < characterSpritesCount; i++) {
    if (characterSprites[i].hash == hash) {
      cp = &characterSprites[i];
      break;
    }
  }
  if (cp == NULL) {
    cp = &characterSprites[characterSpritesCount];
    cp->hash = hash;
    createCharacterImageData(grid);
    cp->sprite = new LGFX_Sprite(&canvas);
    cp->sprite->createSprite(CHARACTER_WIDTH, CHARACTER_HEIGHT);
    cp->sprite->setSwapBytes(true);
    cp->sprite->pushImage(0, 0, CHARACTER_WIDTH, CHARACTER_HEIGHT,
                          characterImageData);
    characterSpritesCount++;
  }
  cp->sprite->pushSprite((int)x, (int)y, TRANSPARENT_COLOR);
}

void md_clearView(unsigned char r, unsigned char g, unsigned char b) {
  canvas.fillSprite(lcd.color565(r, g, b));
}

void md_clearScreen(unsigned char r, unsigned char g, unsigned char b) {
  lcd.fillScreen(lcd.color565(r, g, b));
}

void md_playTone(float freq, float duration, float when) {
  addSoundTone(freq, duration, when);
}

void md_stopTone() {
  initSoundTones();
  M5.Beep.mute();
}

float md_getAudioTime() { return soundTime; }

void md_consoleLog(char *msg) { Serial.println(msg); }

void md_initView(int w, int h) {
  if (isCanvasCreated) {
    canvas.deleteSprite();
  }
  isCanvasCreated = true;
  canvas.createSprite(w, h);
  if (w > 135) {
    lcd.setRotation(1);
  } else {
    lcd.setRotation(0);
  }
  canvasX = (lcd.width() - w) / 2;
  canvasY = (lcd.height() - h) / 2;
}

static TaskHandle_t frameTaskHandle;

static void updateFromFrameTask() {
  bool ba = !lgfx::gpio_in(BUTTON_A_PIN);
  bool bb = !lgfx::gpio_in(BUTTON_B_PIN);
  setButtonState(false, false, false, false, bb, ba);
  if (input.b.isJustPressed) {
    toggleSound();
  }
  updateFrame();
  lcd.startWrite();
  canvas.pushSprite(canvasX, canvasY);
  lcd.endWrite();
}

static void updateFrameTask(void *pvParameters) {
  while (1) {
    xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
    updateFromFrameTask();
  }
}

static void IRAM_ATTR onFrameTimer() {
  xTaskNotifyFromISR(frameTaskHandle, 0, eIncrement, NULL);
}

static TaskHandle_t soundTaskHandle;

static void updateFromSoundTask() {
  M5.Beep.update();
  soundTime += 60 / tempo / TONE_PER_NOTE;
  float lastWhen = 0;
  int ti = -1;
  for (int i = 0; i < SOUND_TONE_COUNT; i++) {
    SoundTone *st = &soundTones[i];
    if (st->when <= soundTime) {
      if (st->when > lastWhen) {
        ti = i;
        lastWhen = st->when;
        st->when = FLT_MAX;
      }
    }
  }
  if (ti >= 0) {
    SoundTone *st = &soundTones[ti];
    M5.Beep.tone((uint16_t)st->freq, (uint32_t)(st->duration * 1000));
  }
}

static void updateSoundTask(void *pvParameters) {
  while (1) {
    xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
    updateFromSoundTask();
  }
}

static void IRAM_ATTR onSoundTimer() {
  xTaskNotifyFromISR(soundTaskHandle, 0, eIncrement, NULL);
}

void setup() {
  M5.begin();
  lcd.init();
  lcd.setRotation(0);
  lcd.setBrightness(128);
  initSoundTones();
  disableSound();
  initCharacterSprite();
  initGame();
  hw_timer_t *frameTimer = NULL;
  frameTimer = timerBegin(0, getApbFrequency() / FPS / 1000, true);
  timerAttachInterrupt(frameTimer, &onFrameTimer, true);
  timerAlarmWrite(frameTimer, 1000, true);
  timerAlarmEnable(frameTimer);
  xTaskCreateUniversal(updateFrameTask, "updateFrameTask", 8192, NULL, 1,
                       &frameTaskHandle, APP_CPU_NUM);
  hw_timer_t *soundTimer = NULL;
  soundTimer = timerBegin(
      1, getApbFrequency() * (60.0f / tempo / TONE_PER_NOTE) / 1000, true);
  timerAttachInterrupt(soundTimer, &onSoundTimer, true);
  timerAlarmWrite(soundTimer, 1000, true);
  timerAlarmEnable(soundTimer);
  xTaskCreateUniversal(updateSoundTask, "updateSoundTask", 8192, NULL, 2,
                       &soundTaskHandle, PRO_CPU_NUM);
}

void loop() { delay(1000); }
