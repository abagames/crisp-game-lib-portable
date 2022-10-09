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
int canvasX;
int canvasY;
uint16_t textImageData[TEXT_PATTERN_COUNT][CHARACTER_WIDTH * CHARACTER_HEIGHT];
LGFX_Sprite *textSprites[TEXT_PATTERN_COUNT];
uint16_t characterImageData[MAX_CHARACTER_PATTERN_COUNT]
                           [CHARACTER_WIDTH * CHARACTER_HEIGHT];
LGFX_Sprite *characterSprites[MAX_CHARACTER_PATTERN_COUNT];

typedef struct {
  float freq;
  float duration;
  float when;
} SoundTone;

#define TONE_PER_NOTE 32
#define SOUND_TONE_COUNT 64
SoundTone soundTones[SOUND_TONE_COUNT];
int soundToneIndex = 0;
float soundTime = 0;

void initSoundTones() {
  for (int i = 0; i < SOUND_TONE_COUNT; i++) {
    soundTones[i].when = FLT_MAX;
  }
}

void addSoundTone(float freq, float duration, float when) {
  SoundTone *st = &soundTones[soundToneIndex];
  st->freq = freq;
  st->duration = duration;
  st->when = when;
  soundToneIndex++;
  if (soundToneIndex >= SOUND_TONE_COUNT) {
    soundToneIndex = 0;
  }
}

#define TRANSPARENT_COLOR 0
uint16_t currentColor;

void md_color(unsigned char r, unsigned char g, unsigned char b) {
  currentColor = lcd.color565(r, g, b);
}

void md_rect(float x, float y, float w, float h) {
  canvas.fillRect((int)x, (int)y, (int)w, (int)h, currentColor);
}

void md_text(char l, float x, float y) {
  textSprites[l - '!']->pushSprite((int)(x - CHARACTER_WIDTH / 2),
                                   (int)(y - CHARACTER_HEIGHT / 2),
                                   TRANSPARENT_COLOR);
}

void md_character(char l, float x, float y) {
  characterSprites[l - 'a']->pushSprite((int)(x - CHARACTER_WIDTH / 2),
                                        (int)(y - CHARACTER_HEIGHT / 2),
                                        TRANSPARENT_COLOR);
}

void md_clearView(unsigned char r, unsigned char g, unsigned char b) {
  uint16_t cc = currentColor;
  md_color(r, g, b);
  canvas.fillSprite(currentColor);
  currentColor = cc;
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

void md_setTexts(char grid[][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1],
                 int count) {
  for (int i = 0; i < count; i++) {
    int cp = 0;
    uint16_t imageData[CHARACTER_WIDTH * CHARACTER_HEIGHT];
    for (int y = 0; y < CHARACTER_HEIGHT; y++) {
      for (int x = 0; x < CHARACTER_WIDTH; x++) {
        imageData[cp] =
            grid[i][y][x] == BLACK ? lcd.color565(9, 9, 9) : TRANSPARENT_COLOR;
        cp++;
      }
    }
    textSprites[i] = new LGFX_Sprite(&canvas);
    textSprites[i]->createSprite(CHARACTER_WIDTH, CHARACTER_HEIGHT);
    textSprites[i]->pushImage(0, 0, CHARACTER_WIDTH, CHARACTER_HEIGHT,
                              imageData);
  }
}

void md_setCharacters(char grid[][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1],
                      int count) {
  for (int i = 0; i < count; i++) {
    int cp = 0;
    uint16_t imageData[CHARACTER_WIDTH * CHARACTER_HEIGHT];
    for (int y = 0; y < CHARACTER_HEIGHT; y++) {
      for (int x = 0; x < CHARACTER_WIDTH; x++) {
        imageData[cp] =
            grid[i][y][x] == BLACK ? lcd.color565(9, 9, 9) : TRANSPARENT_COLOR;
        cp++;
      }
    }
    characterSprites[i] = new LGFX_Sprite(&canvas);
    characterSprites[i]->createSprite(CHARACTER_WIDTH, CHARACTER_HEIGHT);
    characterSprites[i]->pushImage(0, 0, CHARACTER_WIDTH, CHARACTER_HEIGHT,
                                   imageData);
  }
}

void initCanvas() {
  canvas.createSprite(options.viewSizeX, options.viewSizeY);
  canvasX = (lcd.width() - options.viewSizeX) / 2;
  canvasY = (lcd.height() - options.viewSizeY) / 2;
}

TaskHandle_t frameTaskHandle;
bool btnAIsPressed = true;
bool btnAWasPressed = false;
bool btnAWasReleased = false;
bool btnBIsPressed = true;
bool btnBWasPressed = false;

void updateFromTask() {
  bool ba = !lgfx::gpio_in(BUTTON_A_PIN);
  btnAWasPressed = !btnAIsPressed && ba;
  btnAWasReleased = btnAIsPressed && !ba;
  btnAIsPressed = ba;
  setInput(btnAIsPressed, btnAWasPressed, btnAWasReleased);
  bool bb = !lgfx ::gpio_in(BUTTON_B_PIN);
  btnBWasPressed = !btnBIsPressed && bb;
  btnBIsPressed = bb;
  if (btnBWasPressed) {
    toggleSound();
  }
  updateFrame();
  lcd.startWrite();
  canvas.pushSprite(canvasX, canvasY);
  lcd.endWrite();
}

void updateFrameTask(void *pvParameters) {
  while (1) {
    xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
    updateFromTask();
  }
}

void IRAM_ATTR onFrameTimer() {
  xTaskNotifyFromISR(frameTaskHandle, 0, eIncrement, NULL);
}

TaskHandle_t soundTaskHandle;

void updateFromSoundTask() {
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

void updateSoundTask(void *pvParameters) {
  while (1) {
    xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
    updateFromSoundTask();
  }
}

void IRAM_ATTR onSoundTimer() {
  xTaskNotifyFromISR(soundTaskHandle, 0, eIncrement, NULL);
}

void setup() {
  M5.begin();
  lcd.init();
  lcd.setRotation(0);
  lcd.setBrightness(128);
  lcd.fillScreen(lcd.color565(0xdd, 0xdd, 0xdd));
  initSoundTones();
  initGame();
  initCanvas();
  disableSound();
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
