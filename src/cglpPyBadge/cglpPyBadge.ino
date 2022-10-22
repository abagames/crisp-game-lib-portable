#define LGFX_AUTODETECT
#define LGFX_USE_V1

#include <Adafruit_Arcada.h>

#include <LGFX_AUTODETECT.hpp>
#include <LovyanGFX.hpp>

Adafruit_Arcada arcada;

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
static unsigned long nextFrameUpdateMicros;
static float buzzerFreq;
static unsigned long buzzerUntil;

static void initSoundTones() {
  for (int i = 0; i < SOUND_TONE_COUNT; i++) {
    soundTones[i].when = FLT_MAX;
  }
  buzzerFreq = buzzerUntil = -1;
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

static void initCharacterSprite() {
  for (int i = 0; i < MAX_CACHED_CHARACTER_PATTERN_COUNT; i++) {
    characterSprites[i].sprite = new LGFX_Sprite(&canvas);
  }
  characterSpritesCount = 0;
}

static void resetCharacterSprite() {
  for (int i = 0; i < characterSpritesCount; i++) {
    characterSprites[i].sprite->deleteSprite();
  }
  characterSpritesCount = 0;
}

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
  arcada.enableSpeaker(false);
}

float md_getAudioTime() { return soundTime; }

void md_consoleLog(char *msg) { Serial.println(msg); }

void md_initView(int w, int h) {
  if (isCanvasCreated) {
    canvas.deleteSprite();
  }
  isCanvasCreated = true;
  canvas.createSprite(w, h);
  canvasX = (lcd.width() - w) / 2;
  canvasY = (lcd.height() - h) / 2;
  resetCharacterSprite();
}

static bool isStartPressed = false;

static void updateFromFrameTask() {
  uint8_t pressedButtons = arcada.readButtons();
  bool left = pressedButtons & ARCADA_BUTTONMASK_LEFT;
  bool right = pressedButtons & ARCADA_BUTTONMASK_RIGHT;
  bool up = pressedButtons & ARCADA_BUTTONMASK_UP;
  bool down = pressedButtons & ARCADA_BUTTONMASK_DOWN;
  bool ba = pressedButtons & ARCADA_BUTTONMASK_A;
  bool bb = pressedButtons & ARCADA_BUTTONMASK_B;
  setButtonState(left, right, up, down, bb, ba);
  updateFrame();
  lcd.startWrite();
  canvas.pushSprite(canvasX, canvasY);
  lcd.endWrite();
  if (!isInMenu) {
    if (!isStartPressed && (pressedButtons & ARCADA_BUTTONMASK_START)) {
      toggleSound();
    }
    if (pressedButtons & ARCADA_BUTTONMASK_SELECT) {
      goToMenu();
    }
  }
  isStartPressed = pressedButtons & ARCADA_BUTTONMASK_START;
}

static void updateFromSoundTask() {
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
    analogWriteResolution(8);
    analogWrite(A0, 0);
    analogWrite(A1, 0);
    arcada.enableSpeaker(true);
    buzzerUntil = micros() + st->duration * 1000000;
    buzzerFreq = st->freq;
  }
}

void setup() {
  Serial.begin(9600);
  arcada.arcadaBegin();
  lcd.init();
  lcd.setRotation(1);
  lcd.setBrightness(128);
  initCharacterSprite();
  initSoundTones();
  disableSound();
  initGame();
  arcada.timerCallback(tempo / 60.0f * TONE_PER_NOTE, updateFromSoundTask);
  nextFrameUpdateMicros = micros();
}

void loop() {
  unsigned long cm = micros();
  if (buzzerFreq > 0) {
    if (cm >= buzzerUntil) {
      arcada.enableSpeaker(false);
      buzzerFreq = -1;
    } else {
      unsigned long fd = 1000000ul / buzzerFreq;
      unsigned long fr = cm % fd;
      unsigned char v = fr < fd / 2 ? 9 : 0;
      analogWriteResolution(8);
      analogWrite(A0, v);
      analogWrite(A1, v);
    }
  }
  if (cm < nextFrameUpdateMicros) {
    return;
  }
  nextFrameUpdateMicros = cm + 1000000ul / 60;
  updateFromFrameTask();
}
