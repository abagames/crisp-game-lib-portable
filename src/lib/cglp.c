/// \cond
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "cglp.h"
#include "machineDependent.h"
#include "menu.h"
#include "particle.h"
#include "random.h"
#include "sound.h"
#include "textPattern.h"
#include "vector.h"

#define STATE_TITLE 0
#define STATE_IN_GAME 1
#define STATE_GAME_OVER 2
/// \endcond

//! A variable incremented by one every 1/60 of a second (readonly).
int ticks;
//! Game score.
float score;
//! A variable that is one at the beginning of the game, two after 1 minute, and
//! increasing by one every minute (readonly).
float difficulty;
//! Set the color for drawing rectangles, particles, texts, and characters.
//! Setting the color prior to `text()` and `character()` will recolor the
//! pixel art. Use `color = BLACK` to restore and use the original colors.
//! Colors are `WHITE`, `RED`, `GREEN`, `YELLOW`, `BLUE`, `PURPLE`, `CYAN`,
//! `BLACK`, `LIGHT_*` and `TRANSPARENT`.
int color;
//! Set the thickness for drawing `line()`, `bar()` and `arc()`.
float thickness;
//! A value from 0 to 1 that defines where the center coordinates are on the
//! `bar()`, default: 0.5.
float barCenterPosRatio;
//! Options for drawing `text()` and `character()`.
CharacterOptions characterOptions;
//! If `hasCollision` is set to `true`, the drawn rectangle will have no
//! collision with other rectangles. Used to improve performance.
bool hasCollision;
//! Tempo of background music.
float tempo = 120;
//! Input state from the buttons (readonly).
Input input;
//! Input state (used to get input on title screen) (readonly).
Input currentInput;
//! Set to `true` when the menu screen is open (readonly).
bool isInMenu;

static int state;
static bool hasTitle;
static bool isShowingScore;
static bool isBgmEnabled;

static char *title;
static char *description;
static char (*characters)[CHARACTER_HEIGHT][CHARACTER_WIDTH + 1];
static int charactersCount;
static Options options;
static void (*update)(void);

// Collision
/// \cond
typedef struct {
  int rectIndex;
  int textIndex;
  int characterIndex;
  Vector pos;
  Vector size;
} HitBox;

#define MAX_HIT_BOX_COUNT 256
/// \endcond
static HitBox hitBoxes[MAX_HIT_BOX_COUNT];
static int hitBoxesIndex;

static void initCollision(Collision *collision) {
  for (int i = 0; i < COLOR_COUNT; i++) {
    collision->isColliding.rect[i] = false;
  }
  for (int i = '!'; i <= '~'; i++) {
    collision->isColliding.text[i] = false;
  }
  for (int i = 'a'; i <= 'z'; i++) {
    collision->isColliding.character[i] = false;
  }
}

static bool testCollision(HitBox r1, HitBox r2) {
  int ox = r2.pos.x - r1.pos.x;
  int oy = r2.pos.y - r1.pos.y;
  return -r2.size.x < ox && ox < r1.size.x && -r2.size.y < oy && oy < r1.size.y;
}

static void checkHitBox(Collision *cl, HitBox hitBox) {
  for (int i = 0; i < hitBoxesIndex; i++) {
    HitBox hb = hitBoxes[i];
    if (testCollision(hb, hitBox)) {
      if (hb.rectIndex >= 0) {
        cl->isColliding.rect[hb.rectIndex] = true;
      }
      if (hb.textIndex >= 0) {
        cl->isColliding.text[hb.textIndex] = true;
      }
      if (hb.characterIndex >= 0) {
        cl->isColliding.character[hb.characterIndex] = true;
      }
    }
  }
}

// Drawing
/// \cond
#define MAX_DRAWING_HIT_BOXES_COUNT 64
/// \endcond
static HitBox drawingHitBoxes[MAX_DRAWING_HIT_BOXES_COUNT];
static int drawingHitBoxesIndex;

static void beginAddingRects() { drawingHitBoxesIndex = 0; }

static void addRect(bool isAlignCenter, float x, float y, float w, float h,
                    Collision *hitCollision) {
  if (isAlignCenter) {
    x -= w / 2;
    y -= h / 2;
  }
  if (hasCollision) {
    HitBox hb;
    hb.rectIndex = color;
    hb.textIndex = hb.characterIndex = -1;
    hb.pos.x = floor(x);
    hb.pos.y = floor(y);
    hb.size.x = floor(w);
    hb.size.y = floor(h);
    checkHitBox(hitCollision, hb);
    if (color > TRANSPARENT &&
        drawingHitBoxesIndex < MAX_DRAWING_HIT_BOXES_COUNT) {
      drawingHitBoxes[drawingHitBoxesIndex] = hb;
      drawingHitBoxesIndex++;
    }
  }
  if (color > TRANSPARENT) {
    ColorRgb *rgb = &colorRgbs[color];
    md_drawRect(x, y, w, h, rgb->r, rgb->g, rgb->b);
  }
}

static bool isShownTooManyHitBoxesMessage = false;

static void addHitBox(HitBox hb) {
  if (hitBoxesIndex < MAX_HIT_BOX_COUNT) {
    hitBoxes[hitBoxesIndex] = hb;
    hitBoxesIndex++;
  } else {
    if (!isShownTooManyHitBoxesMessage) {
      consoleLog("too many hit boxes");
      isShownTooManyHitBoxesMessage = true;
    }
  }
}

static void endAddingRects() {
  for (int i = 0; i < drawingHitBoxesIndex; i++) {
    addHitBox(drawingHitBoxes[i]);
  }
}

//! Draw a rectangle. Returns information on objects that collided while
//! drawing.
Collision rect(float x, float y, float w, float h) {
  Collision hitCollision;
  initCollision(&hitCollision);
  beginAddingRects();
  addRect(false, x, y, w, h, &hitCollision);
  endAddingRects();
  return hitCollision;
}

//! Draw a box. Returns information on objects that collided while drawing.
Collision box(float x, float y, float w, float h) {
  Collision hitCollision;
  initCollision(&hitCollision);
  beginAddingRects();
  addRect(true, x, y, w, h, &hitCollision);
  endAddingRects();
  return hitCollision;
}

static void drawLine(float x, float y, float ox, float oy,
                     Collision *hitCollision) {
  int t = floor(thickness);
  if (t < 3) {
    t = 3;
  } else if (t > 10) {
    t = 10;
  }
  float lx = fabsf(ox);
  float ly = fabsf(oy);
  float rn = ceilf(lx > ly ? lx / t : ly / t);
  if (rn < 3) {
    rn = 3;
  } else if (rn > 99) {
    rn = 99;
  }
  ox /= (rn - 1);
  oy /= (rn - 1);
  for (int i = 0; i < rn; i++) {
    addRect(true, x, y, thickness, thickness, hitCollision);
    x += ox;
    y += oy;
  }
}

//! Draw a line. Returns information on objects that collided while drawing.
Collision line(float x1, float y1, float x2, float y2) {
  Collision hitCollision;
  initCollision(&hitCollision);
  beginAddingRects();
  drawLine(x1, y1, x2 - x1, y2 - y1, &hitCollision);
  endAddingRects();
  return hitCollision;
}

//! Draw a bar. Returns information on objects that collided while drawing.
Collision bar(float x, float y, float length, float angle) {
  Collision hitCollision;
  initCollision(&hitCollision);
  Vector l;
  rotate(vectorSet(&l, length, 0), angle);
  Vector p;
  vectorSet(&p, x - l.x * barCenterPosRatio, y - l.y * barCenterPosRatio);
  beginAddingRects();
  drawLine(p.x, p.y, l.x, l.y, &hitCollision);
  endAddingRects();
  return hitCollision;
}

//! Draw a arc. Returns information on objects that collided while drawing.
Collision arc(float centerX, float centerY, float radius, float angleFrom,
              float angleTo) {
  Collision hitCollision;
  initCollision(&hitCollision);
  beginAddingRects();
  float af, ao;
  if (angleFrom > angleTo) {
    af = angleTo;
    ao = angleFrom - angleTo;
  } else {
    af = angleFrom;
    ao = angleTo - angleFrom;
  }
  if (ao < 0.01f) {
    return hitCollision;
  }
  if (ao < 0) {
    ao = 0;
  } else if (ao > M_PI * 2) {
    ao = M_PI * 2;
  }
  int lc = ceilf(ao * sqrtf(radius * 0.25f));
  if (lc < 1) {
    lc = 1;
  } else if (lc > 36) {
    lc = 36;
  }
  float ai = ao / lc;
  float a = af;
  float p1x = radius * cosf(a) + centerX;
  float p1y = radius * sinf(a) + centerY;
  float p2x, p2y;
  float ox, oy;
  for (int i = 0; i < lc; i++) {
    a += ai;
    p2x = radius * cosf(a) + centerX;
    p2y = radius * sinf(a) + centerY;
    ox = p2x - p1x;
    oy = p2y - p1y;
    drawLine(p1x, p1y, ox, oy, &hitCollision);
    p1x = p2x;
    p1y = p2y;
  }
  endAddingRects();
  return hitCollision;
}

// Text and character
/// \cond
typedef struct {
  unsigned char grid[CHARACTER_HEIGHT][CHARACTER_WIDTH][3];
  HitBox hitBox;
  int hash;
} CharacterPattern;
/// \endcond

static CharacterPattern characterPatterns[MAX_CACHED_CHARACTER_PATTERN_COUNT];
static int characterPatternsCount;

static void initCharacter() {
  characterPatternsCount = 0;
  characterOptions.isMirrorX = characterOptions.isMirrorY = false;
  characterOptions.rotation = 0;
}

static char *colorGridChars = "wrgybpclRGYBPCL";

static void setColorGrid(
    char grid[CHARACTER_HEIGHT][CHARACTER_WIDTH + 1],
    unsigned char colorGrid[CHARACTER_HEIGHT][CHARACTER_WIDTH][3],
    int colorIndex, HitBox *hb) {
  Vector leftTop;
  vectorSet(&leftTop, CHARACTER_WIDTH, CHARACTER_HEIGHT);
  Vector rightBottom;
  vectorSet(&rightBottom, 0, 0);
  for (int y = 0; y < CHARACTER_HEIGHT; y++) {
    for (int x = 0; x < CHARACTER_WIDTH; x++) {
      int gx = x;
      int gy = y;
      if (characterOptions.isMirrorX) {
        gx = CHARACTER_WIDTH - gx - 1;
      }
      if (characterOptions.isMirrorY) {
        gy = CHARACTER_HEIGHT - gy - 1;
      }
      if (characterOptions.rotation == 1) {
        int tx = gx;
        gx = gy;
        gy = CHARACTER_WIDTH - tx - 1;
      } else if (characterOptions.rotation == 2) {
        gx = CHARACTER_WIDTH - gx - 1;
        gy = CHARACTER_HEIGHT - gy - 1;
      } else if (characterOptions.rotation == 3) {
        int tx = gx;
        gx = CHARACTER_HEIGHT - gy - 1;
        gy = tx;
      }
      char *ci = strchr(colorGridChars, grid[gy][gx]);
      if (ci == NULL) {
        colorGrid[y][x][0] = colorGrid[y][x][1] = colorGrid[y][x][2] = 0;
      } else {
        ColorRgb *rgb = colorIndex == BLACK ? &colorRgbs[ci - colorGridChars]
                                            : &colorRgbs[colorIndex];
        colorGrid[y][x][0] = rgb->r;
        colorGrid[y][x][1] = rgb->g;
        colorGrid[y][x][2] = rgb->b;
        if (x < leftTop.x) {
          leftTop.x = x;
        }
        if (y < leftTop.y) {
          leftTop.y = y;
        }
        if (x > rightBottom.x) {
          rightBottom.x = x;
        }
        if (y > rightBottom.y) {
          rightBottom.y = y;
        }
      }
    }
  }
  vectorSet(&hb->pos, leftTop.x, leftTop.y);
  vectorSet(&hb->size, clamp(rightBottom.x - leftTop.x + 1, 0, CHARACTER_WIDTH),
            clamp(rightBottom.y - leftTop.y + 1, 0, CHARACTER_HEIGHT));
}

static int getHashFromIntegers(int *values, int count) {
  int hash = 23;
  for (int i = 0; i < count; i++) {
    hash = hash * 37 + values[i];
  }
  return hash;
}

static bool isShownTooManyCharacterPatternsMessage = false;

static void drawCharacter(int index, float x, float y, bool _hasCollision,
                          bool isText, Collision *hitCollision) {
  if ((isText && (index < '!' || index > '~')) ||
      (!isText && (index < 'a' || index > 'z'))) {
    return;
  }
  int hashBaseValues[7] = {index,
                           isText,
                           color,
                           characterOptions.isMirrorX,
                           characterOptions.isMirrorY,
                           characterOptions.rotation};
  int hash = getHashFromIntegers(hashBaseValues, 7);
  CharacterPattern *cp = NULL;
  for (int i = 0; i < characterPatternsCount; i++) {
    if (characterPatterns[i].hash == hash) {
      cp = &characterPatterns[i];
      break;
    }
  }
  if (cp == NULL) {
    if (characterPatternsCount >= MAX_CACHED_CHARACTER_PATTERN_COUNT) {
      if (!isShownTooManyCharacterPatternsMessage) {
        consoleLog("too many charater patterns");
        isShownTooManyCharacterPatternsMessage = true;
      }
      return;
    }
    cp = &characterPatterns[characterPatternsCount];
    cp->hash = hash;
    setColorGrid(isText ? textPatterns[index - '!'] : characters[index - 'a'],
                 cp->grid, color, &cp->hitBox);
    characterPatternsCount++;
  }
  if (color > TRANSPARENT) {
    md_drawCharacter(cp->grid, x, y, hash);
  }
  if (hasCollision && _hasCollision) {
    HitBox *thb = &cp->hitBox;
    HitBox hb;
    hb.textIndex = isText ? index : -1;
    hb.characterIndex = !isText ? index : -1;
    hb.rectIndex = -1;
    hb.pos.x = floor(x + thb->pos.x);
    hb.pos.y = floor(y + thb->pos.y);
    hb.size.x = thb->size.x;
    hb.size.y = thb->size.y;
    checkHitBox(hitCollision, hb);
    if (color > TRANSPARENT) {
      addHitBox(hb);
    }
  }
}

static Collision drawCharacters(char *msg, float x, float y, bool _hasCollision,
                                bool isText) {
  Collision hitCollision;
  initCollision(&hitCollision);
  int ml = strlen(msg);
  x -= CHARACTER_WIDTH / 2;
  y -= CHARACTER_HEIGHT / 2;
  for (int i = 0; i < ml; i++) {
    drawCharacter(msg[i], x, y, _hasCollision, isText, &hitCollision);
    x += 6;
  }
  return hitCollision;
}

//! Draw a text. Returns information on objects that collided while drawing.
Collision text(char *msg, float x, float y) {
  return drawCharacters(msg, x, y, true, true);
}

//! Draw a pixel art. Returns information on objects that collided while
//! drawing. You can define pixel arts (6x6 dots) of characters with
//! `characters` array.
Collision character(char *msg, float x, float y) {
  return drawCharacters(msg, x, y, true, false);
}

// Color
/// \cond
ColorRgb colorRgbs[COLOR_COUNT];
/// \endcond
static ColorRgb whiteRgb;

static ColorRgb getRgb(int index, bool isDarkColor) {
  int rgbValues[] = {
      0xeeeeee, 0xe91e63, 0x4caf50, 0xffc107,
      0x3f51b5, 0x9c27b0, 0x03a9f4, 0x616161,
  };
  int i = index >= LIGHT_RED ? index - LIGHT_RED + RED : index;
  if (isDarkColor) {
    if (i == 0) {
      i = 7;
    } else if (i == 7) {
      i = 0;
    }
  }
  int v = rgbValues[i];
  ColorRgb cr;
  cr.r = (v & 0xff0000) >> 16;
  cr.g = (v & 0xff00) >> 8;
  cr.b = v & 0xff;
  if (index >= LIGHT_RED) {
    cr.r = isDarkColor ? cr.r / 2 : whiteRgb.r - (whiteRgb.r - cr.r) / 2;
    cr.g = isDarkColor ? cr.g / 2 : whiteRgb.g - (whiteRgb.g - cr.g) / 2;
    cr.b = isDarkColor ? cr.b / 2 : whiteRgb.b - (whiteRgb.b - cr.b) / 2;
  }
  if (index == WHITE) {
    whiteRgb = cr;
  }
  return cr;
}

static void initColor() {
  bool isDarkColor = options.isDarkColor;
  for (int i = 0; i < COLOR_COUNT; i++) {
    colorRgbs[i] = getRgb(i, isDarkColor);
  }
  if (isDarkColor) {
    colorRgbs[WHITE].r = colorRgbs[BLUE].r / 6;
    colorRgbs[WHITE].g = colorRgbs[BLUE].g / 6;
    colorRgbs[WHITE].b = colorRgbs[BLUE].b / 6;
  }
}

static void clearView() {
  md_clearView(colorRgbs[WHITE].r, colorRgbs[WHITE].g, colorRgbs[WHITE].b);
}

static int savedColor;
static CharacterOptions savedCharacterOptions;

static void resetColorAndCharacterOptions() {
  color = BLACK;
  characterOptions.isMirrorX = characterOptions.isMirrorY = false;
  characterOptions.rotation = 0;
}

static void saveCurrentColorAndCharacterOptions() {
  savedColor = color;
  savedCharacterOptions = characterOptions;
  resetColorAndCharacterOptions();
}

static void loadCurrentColorAndCharacterOptions() {
  color = savedColor;
  characterOptions = savedCharacterOptions;
}

// Sound
//! Play a sound effect. Type are `COIN`, `LASER`, `EXPLOSION`, `POWER_UP`,
//! `HIT`, `JUMP`, `SELECT`, `CLICK` and `RANDOM`.
void play(int type) { playSoundEffect(type); }

//! Enable playing background music and sound effects.
void enableSound() { isSoundEnabled = true; }

//! Disable playing background music and sound effects.
void disableSound() {
  isSoundEnabled = false;
  md_stopTone();
}

//! Toggle sound playing flag.
void toggleSound() {
  if (isSoundEnabled) {
    disableSound();
  } else {
    enableSound();
  }
}

// Score
static int prevScore;
static int hiScore;

/// \cond
typedef struct {
  char str[9];
  Vector pos;
  float vy;
  int ticks;
} ScoreBoard;

#define MAX_SCORE_BOARD_COUNT 16
/// \endcond
static ScoreBoard scoreBoards[MAX_SCORE_BOARD_COUNT];
static int scoreBoardsIndex;

static void initScore() { score = prevScore = hiScore = 0; }

static void initScoreBoards() {
  for (int i = 0; i < MAX_SCORE_BOARD_COUNT; i++) {
    scoreBoards[i].ticks = 0;
  }
  scoreBoardsIndex = 0;
}

static void updateScoreBoards() {
  saveCurrentColorAndCharacterOptions();
  for (int i = 0; i < MAX_SCORE_BOARD_COUNT; i++) {
    ScoreBoard *sb = &scoreBoards[i];
    if (sb->ticks > 0) {
      drawCharacters(sb->str, sb->pos.x, sb->pos.y, false, true);
      sb->pos.y += sb->vy;
      sb->vy *= 0.9f;
      sb->ticks--;
    }
  }
  loadCurrentColorAndCharacterOptions();
}

//! Add score points and draw additional score on the screen. You can also add
//! score points simply by adding the `score` variable.
void addScore(float value, float x, float y) {
  score += value;
  int v = (int)value;
  ScoreBoard *sb = &scoreBoards[scoreBoardsIndex];
  sprintf(sb->str, v > 0 ? "+%d" : "%d", v);
  int l = strlen(sb->str);
  sb->pos.x = x - l * CHARACTER_WIDTH / 2;
  sb->pos.y = y - CHARACTER_HEIGHT / 2;
  sb->vy = -2;
  sb->ticks = 30;
  scoreBoardsIndex++;
  if (scoreBoardsIndex >= MAX_SCORE_BOARD_COUNT) {
    scoreBoardsIndex = 0;
  }
}

static void drawScore() {
  if (!isShowingScore) {
    return;
  }
  saveCurrentColorAndCharacterOptions();
  int cc = color;
  color = BLACK;
  char sc[16];
  int s = state == STATE_IN_GAME ? (int)score : prevScore;
  snprintf(sc, 15, "%d", s);
  drawCharacters(sc, 3, 3, false, true);
  snprintf(sc, 15, "HI %d", hiScore);
  drawCharacters(sc, options.viewSizeX - strlen(sc) * 6 + 2, 3, false, true);
  loadCurrentColorAndCharacterOptions();
}

//! Add particles.
void particle(float x, float y, float count, float speed, float angle,
              float angleWidth) {
  addParticle(x, y, count, speed, angle, angleWidth);
}

// Input and replay
static void clearButtonState(ButtonState *bs) {
  bs->isPressed = bs->isJustPressed = bs->isJustReleased = false;
}

static bool isInputJustInitialized;

static void initInput() {
  clearButtonState(&input.left);
  clearButtonState(&input.right);
  clearButtonState(&input.up);
  clearButtonState(&input.down);
  clearButtonState(&input.b);
  clearButtonState(&input.a);
  currentInput = input;
  isInputJustInitialized = true;
}

static void updateButtonState(ButtonState *bs, bool isPressed) {
  bs->isJustPressed = isPressed && !bs->isPressed;
  bs->isJustReleased = !isPressed && bs->isPressed;
  bs->isPressed = isPressed;
}

static void updateButtons(Input *input, bool left, bool right, bool up,
                          bool down, bool b, bool a) {
  updateButtonState(&input->left, left);
  updateButtonState(&input->right, right);
  updateButtonState(&input->up, up);
  updateButtonState(&input->down, down);
  updateButtonState(&input->b, b);
  updateButtonState(&input->a, a);
  bool isPressed = left || right || up || down || b || a;
  input->isJustPressed = isPressed && !input->isPressed;
  input->isJustReleased = !isPressed && input->isPressed;
  input->isPressed = isPressed;
}

//! Set whether or not each button is pressed.
//! This function must be called from the machine dependent input handling step.
EMSCRIPTEN_KEEPALIVE
void setButtonState(bool left, bool right, bool up, bool down, bool b, bool a) {
  updateButtons(&currentInput, left, right, up, down, b, a);
  if (isInputJustInitialized) {
    currentInput.left.isJustReleased = currentInput.right.isJustReleased =
        currentInput.up.isJustReleased = currentInput.down.isJustReleased =
            currentInput.b.isJustReleased = currentInput.a.isJustReleased =
                false;
    isInputJustInitialized = false;
  }
}

static void updateInput() { input = currentInput; }

/// \cond
#define MAX_RECORDED_INPUT_COUNT 5120
/// \endcond
static uint32_t replayRandomSeed;
static uint8_t recordedInputs[MAX_RECORDED_INPUT_COUNT];
static int recordedInputCount;
static int recordedInputIndex;
static bool isReplayRecorded;
static bool isReplaying;
static Random gameRandom;

static void initReplay() { isReplayRecorded = isReplaying = false; }

static void recordInput() {
  if (recordedInputCount >= MAX_RECORDED_INPUT_COUNT) {
    return;
  }
  recordedInputs[recordedInputCount] =
      (input.left.isPressed << 5) | (input.right.isPressed << 4) |
      (input.up.isPressed << 3) | (input.down.isPressed << 2) |
      (input.b.isPressed << 1) | input.a.isPressed;
  recordedInputCount++;
}

static void initRecord(uint32_t randomSeed) {
  replayRandomSeed = randomSeed;
  setRandomSeed(&gameRandom, randomSeed);
  recordedInputCount = 0;
  isReplayRecorded = true;
  updateInput();
  recordInput();
}

static bool replayInput() {
  if (recordedInputIndex >= recordedInputCount) {
    return false;
  }
  uint8_t ri = recordedInputs[recordedInputIndex];
  bool left = (ri & 0x20) >> 5;
  bool right = (ri & 0x10) >> 4;
  bool up = (ri & 0x8) >> 3;
  bool down = (ri & 0x4) >> 2;
  bool b = (ri & 0x2) >> 1;
  bool a = ri & 0x1;
  updateButtons(&input, left, right, up, down, b, a);
  recordedInputIndex++;
  return true;
}

static void startReplay() {
  setRandomSeed(&gameRandom, replayRandomSeed);
  recordedInputIndex = 0;
  isReplaying = true;
  replayInput();
}

static void stopReplay() { isReplaying = false; }

// Utilities
//! Get a random float value of [low, high).
float rnd(float low, float high) { return getRandom(&gameRandom, low, high); }

//! Get a random int value of [low, high - 1].
int rndi(int low, int high) { return getIntRandom(&gameRandom, low, high); }

//! Print a message to the console (the same arguments as for printf can be
//! used).
void consoleLog(char *format, ...) {
  char cc[99];
  va_list args;
  va_start(args, format);
  vsnprintf(cc, 98, format, args);
  md_consoleLog(cc);
}

//! Clamp a value to [low, high].
float clamp(float v, float low, float high) {
  return fmaxf(low, fminf(v, high));
}

//! Wrap a value to [low, high).
float wrap(float v, float low, float high) {
  float w = high - low;
  float o = v - low;
  if (o >= 0) {
    return fmodf(o, w) + low;
  } else {
    float wv = w + fmodf(o, w) + low;
    if (wv >= high) {
      wv -= w;
    }
    return wv;
  }
}

static char numberCharBuffer[99];

//! Convert a value of type `int` to a value of type `char*`.
char *intToChar(int v) {
  snprintf(numberCharBuffer, 98, "%d", v);
  return numberCharBuffer;
}

// In game
static Random gameSeedRandom;

static void resetDrawState() {
  resetColorAndCharacterOptions();
  thickness = 3;
  barCenterPosRatio = 0.5f;
  hasCollision = true;
}

static void initInGame() {
  state = STATE_IN_GAME;
  stopReplay();
  if (prevScore > hiScore) {
    hiScore = prevScore;
  }
  score = 0;
  initScoreBoards();
  initParticle();
  resetDrawState();
  if (isBgmEnabled) {
    isPlayingBgm = true;
  }
  ticks = -1;
  initRecord(nextRandom(&gameSeedRandom));
}

static void updateInGame() {
  clearView();
  updateInput();
  recordInput();
  update();
  updateParticles();
  updateScoreBoards();
}

// Title
/// \cond
#define MAX_DESCRIPTION_LINE_COUNT 5
#define MAX_DESCRIPTION_STRLEN 32
/// \endcond
static char descriptions[MAX_DESCRIPTION_LINE_COUNT]
                        [MAX_DESCRIPTION_STRLEN + 1];
static int descriptionLineCount;
static int descriptionX;

static void parseDescription() {
  descriptionLineCount = 0;
  int dl = 0;
  char *line = description;
  while (strlen(line) > 0) {
    char *ni = strchr(line, '\n');
    int ll = ni == NULL ? strlen(line) : ni - line;
    int lln = ll;
    if (lln > MAX_DESCRIPTION_STRLEN) {
      lln = MAX_DESCRIPTION_STRLEN;
    }
    strncpy(descriptions[descriptionLineCount], line, lln);
    descriptions[descriptionLineCount][lln] = '\0';
    if (lln > dl) {
      dl = lln;
    }
    descriptionLineCount++;
    if (descriptionLineCount >= MAX_DESCRIPTION_LINE_COUNT) {
      break;
    }
    if (ni == NULL) {
      break;
    }
    line += ll + 1;
  };
  descriptionX = (options.viewSizeX - dl * CHARACTER_WIDTH) / 2;
}

static void initTitle() {
  state = STATE_TITLE;
  ticks = -1;
  resetDrawState();
  if (isReplayRecorded) {
    initParticle();
    resetDrawState();
    startReplay();
  }
}

static void updateTitle() {
  clearView();
  if (currentInput.isJustPressed) {
    initInGame();
    return;
  }
  if (isReplaying) {
    if (!replayInput()) {
      ticks = -1;
      startReplay();
    } else {
      update();
      updateParticles();
    }
  }
  saveCurrentColorAndCharacterOptions();
  drawCharacters(title,
                 (options.viewSizeX - strlen(title) * CHARACTER_WIDTH) / 2,
                 options.viewSizeY * 0.25f, false, true);
  if (ticks > 30) {
    for (int i = 0; i < descriptionLineCount; i++) {
      drawCharacters(descriptions[i], descriptionX,
                     options.viewSizeY * 0.55f + i * CHARACTER_HEIGHT, false,
                     true);
    }
  }
  loadCurrentColorAndCharacterOptions();
}

// Game over
static int gameOverTicks;
static char *gameOverText = "GAME OVER";

static void drawGameOver() {
  drawCharacters(
      gameOverText,
      (options.viewSizeX - strlen(gameOverText) * CHARACTER_WIDTH) / 2,
      options.viewSizeY * 0.5f, false, true);
}

static void initGameOver() {
  state = STATE_GAME_OVER;
  if (isReplaying) {
    stopReplay();
  } else {
    isPlayingBgm = false;
    prevScore = (int)score;
  }
  gameOverTicks = 0;
  saveCurrentColorAndCharacterOptions();
  drawGameOver();
  loadCurrentColorAndCharacterOptions();
}

static void updateGameOver() {
  if (gameOverTicks == 20) {
    saveCurrentColorAndCharacterOptions();
    drawGameOver();
    loadCurrentColorAndCharacterOptions();
  }
  if (gameOverTicks > 20 && currentInput.isJustPressed) {
    initInGame();
  } else if (hasTitle && gameOverTicks > 120) {
    initTitle();
  }
  gameOverTicks++;
}

//! Transit to the game-over state.
void gameOver() { initGameOver(); }

static void resetGame(int gameIndex) {
  Game game = getGame(gameIndex);
  title = game.title;
  description = game.description;
  characters = game.characters;
  charactersCount = game.charactersCount;
  options = game.options;
  update = game.update;
  md_initView(options.viewSizeX, options.viewSizeY);
  initColor();
  initCharacter();
  initScore();
  initParticle();
  initSound(title, description, options.soundSeed);
  initReplay();
  parseDescription();
  setRandomSeedWithTime(&gameSeedRandom);
  unsigned char cc = options.isDarkColor ? 0x10 : 0xe0;
  md_clearScreen(cc, cc, cc);
  resetDrawState();
  hasTitle = strlen(title) + strlen(description) > 0;
  if (hasTitle) {
    initTitle();
  } else {
    initInGame();
  }
  ticks = 0;
}

//! Go to the game selection menu screen.
void goToMenu() {
  isShowingScore = false;
  isBgmEnabled = false;
  isInMenu = true;
  resetGame(0);
}

// Initialize
//! Restart another game specified by `gameIndex`.
void restartGame(int gameIndex) {
  isShowingScore = true;
  isBgmEnabled = true;
  isInMenu = false;
  resetGame(gameIndex);
}

//! Initialize all games and go to the game selection menu screen.
//! This function must be called from the machine dependent setup step.
EMSCRIPTEN_KEEPALIVE
void initGame() {
  initInput();
  addMenu();
  addGames();
  if (gameCount == 2) {
    restartGame(1);
  } else {
    goToMenu();
  }
}

//! Update game frames every 1/60 second.
//! This function must be called from the machine dependent update step.
EMSCRIPTEN_KEEPALIVE
void updateFrame() {
  hitBoxesIndex = 0;
  difficulty = (float)ticks / 60 / FPS + 1;
  if (state == STATE_TITLE) {
    updateTitle();
  } else if (state == STATE_IN_GAME) {
    updateInGame();
  } else if (state == STATE_GAME_OVER) {
    updateGameOver();
  }
  updateSound();
  drawScore();
  ticks++;
  if (currentInput.up.isPressed && currentInput.down.isPressed) {
    if (currentInput.a.isJustPressed) {
      goToMenu();
    }
    if (currentInput.b.isJustPressed) {
      toggleSound();
    }
  }
}
