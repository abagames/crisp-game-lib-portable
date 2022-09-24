#include <M5StickCPlus.h>
#include "cglp.h"
#include "machineDependent.h"

TFT_eSprite canvas = TFT_eSprite(&M5.Lcd);
int canvasX;
int canvasY;
uint16_t textImageData[TEXT_PATTERN_COUNT][CHARACTER_WIDTH * CHARACTER_HEIGHT];
uint16_t characterImageData[MAX_CHARACTER_PATTERN_COUNT][CHARACTER_WIDTH * CHARACTER_HEIGHT];
// TFT_eSprite *characterSprites[MAX_CHARACTER_PATTERN_COUNT];

void md_rect(float x, float y, float w, float h)
{
  canvas.fillRect((int)x, (int)y, (int)w, (int)h, BLACK);
}

void md_text(char l, float x, float y)
{
  canvas.pushImage(
      (int)(x - CHARACTER_WIDTH / 2),
      (int)(y - CHARACTER_HEIGHT / 2),
      CHARACTER_WIDTH,
      CHARACTER_HEIGHT,
      textImageData[l - '!']);
}

void md_character(char l, float x, float y)
{
  canvas.pushImage(
      (int)(x - CHARACTER_WIDTH / 2),
      (int)(y - CHARACTER_HEIGHT / 2),
      CHARACTER_WIDTH,
      CHARACTER_HEIGHT,
      characterImageData[l - 'a']);
  // characterSprites[l - 'a']->pushToSprite(&canvas, x, y, 0xffff);
}

void md_playTone(float freq, float duration)
{
  M5.Beep.tone((int)freq, (int)duration);
}

void md_stopTone()
{
  M5.Beep.mute();
}

void md_consoleLog(char *msg)
{
}

void md_setTexts(char grid[][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1], int count)
{
  for (int i = 0; i < count; i++)
  {
    int cp = 0;
    uint16_t imageData[CHARACTER_WIDTH * CHARACTER_HEIGHT];
    for (int y = 0; y < CHARACTER_HEIGHT; y++)
    {
      for (int x = 0; x < CHARACTER_WIDTH; x++)
      {
        textImageData[i][cp] = grid[i][y][x] == 108 ? 0 : 0xffff;
        cp++;
      }
    }
  }
}

void md_setCharacters(char grid[][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1], int count)
{
  for (int i = 0; i < count; i++)
  {
    int cp = 0;
    uint16_t imageData[CHARACTER_WIDTH * CHARACTER_HEIGHT];
    for (int y = 0; y < CHARACTER_HEIGHT; y++)
    {
      for (int x = 0; x < CHARACTER_WIDTH; x++)
      {
        // RGB565 endian reversed
        characterImageData[i][cp] = grid[i][y][x] == 108 ? 0 : 0xffff;
        // imageData[cp] = grid[i][y][x] == 108 ? 0 : 0xffff;
        cp++;
      }
    }
    // characterSprites[i] = new TFT_eSprite(&M5.Lcd);
    // characterSprites[i]->pushImage(0, 0, CHARACTER_WIDTH, CHARACTER_HEIGHT, imageData);
  }
}

hw_timer_t *timer = NULL;
TaskHandle_t taskHandle;

void updateFromTask()
{
  M5.update();
  setInput(M5.BtnA.isPressed(), M5.BtnA.wasPressed(), M5.BtnA.wasReleased());
  canvas.fillSprite(WHITE);
  updateFrame();
  M5.Lcd.startWrite();
  canvas.pushSprite(canvasX, canvasY);
  M5.Lcd.endWrite();
}

void updateTask(void *pvParameters)
{
  while (1)
  {
    xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
    updateFromTask();
  }
}

void IRAM_ATTR onTimer()
{
  xTaskNotifyFromISR(taskHandle, 0, eIncrement, NULL);
}

void initCanvas()
{
  canvas.createSprite(options.viewSize.x, options.viewSize.y);
  canvasX = (M5.Lcd.width() - options.viewSize.x) / 2;
  canvasY = (M5.Lcd.height() - options.viewSize.y) / 2;
  canvas.setSwapBytes(false);
}

void setup()
{
  M5.begin();
  M5.Axp.ScreenBreath(8);
  M5.Lcd.setSwapBytes(true);
  M5.Lcd.fillScreen(0xdddd);
  initGame();
  initCanvas();
  timer = timerBegin(0, getApbFrequency() / 60 / 1000, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000, true);
  timerAlarmEnable(timer);
  xTaskCreateUniversal(
      updateTask,
      "updateTask",
      8192,
      NULL,
      1,
      &taskHandle,
      APP_CPU_NUM // PRO_CPU_NUM
  );
}

void loop()
{
  delay(1000);
}
