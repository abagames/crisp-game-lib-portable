#include <M5StickCPlus.h>

#include "cglp.h"
#include "machineDependent.h"

TFT_eSprite canvas = TFT_eSprite(&M5.Lcd);
int canvasX;
int canvasY;
uint16_t textImageData[TEXT_PATTERN_COUNT][CHARACTER_WIDTH * CHARACTER_HEIGHT];
uint16_t characterImageData[MAX_CHARACTER_PATTERN_COUNT]
                           [CHARACTER_WIDTH * CHARACTER_HEIGHT];
// TFT_eSprite *characterSprites[MAX_CHARACTER_PATTERN_COUNT];

typedef struct {
  float freq;
  float duration;
  float when;
} SoundTone;

#define SOUND_TONE_COUNT 64
SoundTone soundTones[SOUND_TONE_COUNT];
int soundToneIndex = 0;
int addingSoundToneIndex = 0;
#define TONE_PER_NOTE 32

float soundTime = 0;

void addSoundTone(float freq, float duration, float when) {
  SoundTone *st = &soundTones[addingSoundToneIndex];
  st->freq = freq;
  st->duration = duration;
  st->when = when;
  addingSoundToneIndex++;
  if (addingSoundToneIndex >= SOUND_TONE_COUNT) {
    addingSoundToneIndex = 0;
  }
}

void md_rect(float x, float y, float w, float h) {
  canvas.fillRect((int)x, (int)y, (int)w, (int)h, BLACK);
}

void md_text(char l, float x, float y) {
  canvas.pushImage((int)(x - CHARACTER_WIDTH / 2),
                   (int)(y - CHARACTER_HEIGHT / 2), CHARACTER_WIDTH,
                   CHARACTER_HEIGHT, textImageData[l - '!']);
}

void md_character(char l, float x, float y) {
  canvas.pushImage((int)(x - CHARACTER_WIDTH / 2),
                   (int)(y - CHARACTER_HEIGHT / 2), CHARACTER_WIDTH,
                   CHARACTER_HEIGHT, characterImageData[l - 'a']);
  // characterSprites[l - 'a']->pushToSprite(&canvas, x, y, 0xffff);
}

void md_playTone(float freq, float duration, float when) {
  addSoundTone(freq, duration, when);
}

void md_stopTone() {
  soundToneIndex = addingSoundToneIndex = 0;
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
        textImageData[i][cp] = grid[i][y][x] == 108 ? 0 : 0xffff;
        cp++;
      }
    }
  }
}

void md_setCharacters(char grid[][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1],
                      int count) {
  for (int i = 0; i < count; i++) {
    int cp = 0;
    uint16_t imageData[CHARACTER_WIDTH * CHARACTER_HEIGHT];
    for (int y = 0; y < CHARACTER_HEIGHT; y++) {
      for (int x = 0; x < CHARACTER_WIDTH; x++) {
        // RGB565 endian reversed
        characterImageData[i][cp] = grid[i][y][x] == 108 ? 0 : 0xffff;
        // imageData[cp] = grid[i][y][x] == 108 ? 0 : 0xffff;
        cp++;
      }
    }
    // characterSprites[i] = new TFT_eSprite(&M5.Lcd);
    // characterSprites[i]->pushImage(0, 0, CHARACTER_WIDTH, CHARACTER_HEIGHT,
    // imageData);
  }
}

void initCanvas() {
  canvas.createSprite(options.viewSize.x, options.viewSize.y);
  canvasX = (M5.Lcd.width() - options.viewSize.x) / 2;
  canvasY = (M5.Lcd.height() - options.viewSize.y) / 2;
  canvas.setSwapBytes(false);
}

TaskHandle_t frameTaskHandle;

void updateFromTask() {
  M5.update();
  setInput(M5.BtnA.isPressed(), M5.BtnA.wasPressed(), M5.BtnA.wasReleased());
  if (M5.BtnB.wasPressed()) {
    toggleSound();
  }
  canvas.fillSprite(WHITE);
  updateFrame();
  M5.Lcd.startWrite();
  canvas.pushSprite(canvasX, canvasY);
  M5.Lcd.endWrite();
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
  soundTime += 60 / tempo / TONE_PER_NOTE;
  int i = soundToneIndex;
  float lastWhen = 0;
  while (i != addingSoundToneIndex) {
    SoundTone *st = &soundTones[i];
    if (st->when <= soundTime) {
      if (st->when > lastWhen) {
        M5.Beep.tone((uint16_t)st->freq, (uint32_t)(st->duration * 1000));
        lastWhen = st->when;
      }
      soundToneIndex++;
      if (soundToneIndex >= SOUND_TONE_COUNT) {
        soundToneIndex = 0;
      }
    }
    i++;
    if (i >= SOUND_TONE_COUNT) {
      i = 0;
    }
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
  M5.Axp.ScreenBreath(8);
  M5.Lcd.setSwapBytes(true);
  M5.Lcd.fillScreen(0xdddd);
  initGame();
  initCanvas();
  initSound();
  hw_timer_t *frameTimer = NULL;
  frameTimer = timerBegin(0, getApbFrequency() / FPS / 1000, true);
  timerAttachInterrupt(frameTimer, &onFrameTimer, true);
  timerAlarmWrite(frameTimer, 1000, true);
  timerAlarmEnable(frameTimer);
  xTaskCreateUniversal(updateFrameTask, "updateFrameTask", 8192, NULL, 1,
                       &frameTaskHandle, APP_CPU_NUM);
  hw_timer_t *soundTimer = NULL;
  soundTimer = timerBegin(
      1, getApbFrequency() * (60.0f / tempo / TONE_PER_NOTE / 1000), true);
  timerAttachInterrupt(soundTimer, &onSoundTimer, true);
  timerAlarmWrite(soundTimer, 1000, true);
  timerAlarmEnable(soundTimer);
  xTaskCreateUniversal(updateSoundTask, "updateSoundTask", 8192, NULL, 2,
                       &soundTaskHandle, PRO_CPU_NUM);
}

void loop() { delay(1000); }
