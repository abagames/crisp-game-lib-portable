#include "cglp.h"
#include "machineDependent.h"
#include "pd_api.h"

PlaydateAPI* pd;
static PDSynth* synth;
static bool isDarkColor;
static int baseColor;
static int viewWidth, viewHeight;

typedef struct {
  LCDBitmap* sprite;
  int hash;
} CharaterSprite;
static CharaterSprite characterSprites[MAX_CACHED_CHARACTER_PATTERN_COUNT];
static int characterSpritesCount;

static void initCharacterSprite() {
  for (int i = 0; i < MAX_CACHED_CHARACTER_PATTERN_COUNT; i++) {
    characterSprites[i].sprite =
        pd->graphics->newBitmap(CHARACTER_WIDTH, CHARACTER_HEIGHT, kColorClear);
  }
  characterSpritesCount = 0;
}

static void resetCharacterSprite() { characterSpritesCount = 0; }

static void createCharacterImageData(
    unsigned char grid[CHARACTER_HEIGHT][CHARACTER_WIDTH][3]) {
  pd->graphics->fillRect(0, 0, CHARACTER_WIDTH, CHARACTER_HEIGHT, kColorClear);
  int cp = 0;
  for (int y = 0; y < CHARACTER_HEIGHT; y++) {
    for (int x = 0; x < CHARACTER_WIDTH; x++) {
      unsigned char r = grid[y][x][0];
      unsigned char g = grid[y][x][1];
      unsigned char b = grid[y][x][2];
      if (r > 0 || g > 0 || b > 0) {
        pd->graphics->fillRect(x, y, 1, 1, baseColor);
      }
      cp++;
    }
  }
}

void md_drawCharacter(unsigned char grid[CHARACTER_HEIGHT][CHARACTER_WIDTH][3],
                      float x, float y, int hash) {
  CharaterSprite* cp = NULL;
  for (int i = 0; i < characterSpritesCount; i++) {
    if (characterSprites[i].hash == hash) {
      cp = &characterSprites[i];
      break;
    }
  }
  if (cp == NULL) {
    cp = &characterSprites[characterSpritesCount];
    cp->hash = hash;
    pd->graphics->pushContext(cp->sprite);
    createCharacterImageData(grid);
    pd->graphics->popContext();
    characterSpritesCount++;
  }
  pd->graphics->drawBitmap(cp->sprite, (int)x, (int)y, kBitmapUnflipped);
}

static LCDPattern lcdPattern = {0,    0,    0,    0,    0,    0,    0,    0,
                                0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

static unsigned char rotateLeft(unsigned char num, unsigned char rotation) {
  unsigned char dropped;
  rotation %= 8;
  while (rotation--) {
    dropped = (num >> 7) & 1;
    num = (num << 1) | dropped;
  }
  return num;
}

void md_drawRect(float x, float y, float w, float h, unsigned char r,
                 unsigned char g, unsigned char b) {
  int xi = (int)x;
  int yi = (int)y;
  int wi = (int)w;
  int hi = (int)h;
  if (abs(wi) <= 4 && abs(hi) <= 4) {
    pd->graphics->fillRect(xi, yi, wi, hi, baseColor);
    return;
  }
  unsigned char p;
  for (int i = 0; i < 8; i++) {
    switch (i % 3) {
      case 0:
        p = (r & 0xf0) | (g >> 4);
        break;
      case 1:
        p = (b & 0xf0) | (r >> 4);
        break;
      case 2:
        p = (g & 0xf0) | (b >> 4);
        break;
    }
    lcdPattern[i] = rotateLeft(p ^ 0xff, i * 3);
  }
  pd->graphics->fillRect(xi, yi, wi, hi, &lcdPattern);
}

void md_clearView(unsigned char r, unsigned char g, unsigned char b) {
  pd->graphics->clear(baseColor);
  pd->graphics->fillRect(0, 0, viewWidth, viewHeight,
                         isDarkColor ? kColorBlack : kColorWhite);
}

void md_clearScreen(unsigned char r, unsigned char g, unsigned char b) {
  isDarkColor = r < 128 && g < 182 && b < 128;
  baseColor = isDarkColor ? kColorWhite : kColorBlack;
  pd->graphics->setBackgroundColor(baseColor);
  int scale = 2;
  pd->display->setScale(scale);
  pd->display->setOffset(0, 0);
  pd->graphics->clear(baseColor);
  pd->display->setOffset((pd->display->getWidth() - viewWidth) / 2 * scale,
                         (pd->display->getHeight() - viewHeight) / 2 * scale);
}

void md_playTone(float freq, float duration, float when) {
  pd->sound->synth->playNote(synth, freq, 0.1f, duration,
                             (uint32_t)(when * 44100));
}

void md_stopTone() { pd->sound->synth->noteOff(synth, 0); }

float md_getAudioTime() { return (float)pd->sound->getCurrentTime() / 44100; }

void md_initView(int w, int h) {
  viewWidth = w;
  viewHeight = h;
  resetCharacterSprite();
}

void md_consoleLog(char* msg) { pd->system->logToConsole(msg); }

static PDButtons buttonsCurrent, buttonsPushed, buttonsReleased;

static int update(void* userdata) {
  pd->system->getButtonState(&buttonsCurrent, &buttonsPushed, &buttonsReleased);
  setButtonState(buttonsCurrent & kButtonLeft, buttonsCurrent & kButtonRight,
                 buttonsCurrent & kButtonUp, buttonsCurrent & kButtonDown,
                 buttonsCurrent & kButtonB, buttonsCurrent & kButtonA);
  updateFrame();
  if (!isInMenu) {
    if ((buttonsCurrent & kButtonA) && (buttonsCurrent & kButtonB) &&
        (buttonsCurrent & kButtonUp) && (buttonsCurrent & kButtonRight)) {
      goToMenu();
    }
  }
  return 1;
}

static void init() {
  synth = pd->sound->synth->newSynth();
  pd->display->setRefreshRate(FPS);
  initCharacterSprite();
  initGame();
  pd->system->setUpdateCallback(update, NULL);
}

#ifdef _WINDLL
__declspec(dllexport)
#endif
    int eventHandler(PlaydateAPI* _pd, PDSystemEvent event, uint32_t arg) {
  if (event == kEventInit) {
    pd = _pd;
    init();
  }
  return 0;
}
