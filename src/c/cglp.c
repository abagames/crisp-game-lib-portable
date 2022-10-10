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
#include "particle.h"
#include "random.h"
#include "sound.h"
#include "textPattern.h"
#include "vector.h"

#define STATE_TITLE 0
#define STATE_IN_GAME 1
#define STATE_GAME_OVER 2

int ticks;
float difficulty;
float thickness = 3;
float barCenterPosRatio = 0.5;
float tempo = 120;
Input input;
int currentColorIndex;
int state;
bool hasTitle;
Random gameRandom;

// Collision
typedef struct {
  int rectIndex;
  int textIndex;
  int characterIndex;
  Vector pos;
  Vector size;
} HitBox;

#define MAX_HIT_BOX_COUNT 256
HitBox hitBoxes[MAX_HIT_BOX_COUNT];
int hitBoxesIndex;

void initCollision(Collision *collision) {
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

bool testCollision(HitBox r1, HitBox r2) {
  int ox = r2.pos.x - r1.pos.x;
  int oy = r2.pos.y - r1.pos.y;
  return -r2.size.x < ox && ox < r1.size.x && -r2.size.y < oy && oy < r1.size.y;
}

void checkHitBox(Collision *cl, HitBox hitBox) {
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
#define MAX_DRAWING_HIT_BOXES_COUNT 64
HitBox drawingHitBoxes[MAX_DRAWING_HIT_BOXES_COUNT];
int drawingHitBoxesIndex;

void beginAddingRects() { drawingHitBoxesIndex = 0; }

void addRect(bool isAlignCenter, float x, float y, float w, float h,
             Collision *hitCollision) {
  if (isAlignCenter) {
    x -= w / 2;
    y -= h / 2;
  }
  HitBox hb;
  hb.rectIndex = currentColorIndex;
  hb.textIndex = hb.characterIndex = -1;
  hb.pos.x = floor(x);
  hb.pos.y = floor(y);
  hb.size.x = floor(w);
  hb.size.y = floor(h);
  checkHitBox(hitCollision, hb);
  if (currentColorIndex > TRANSPARENT &&
      drawingHitBoxesIndex < MAX_DRAWING_HIT_BOXES_COUNT) {
    drawingHitBoxes[drawingHitBoxesIndex] = hb;
    drawingHitBoxesIndex++;
  }
  if (currentColorIndex > TRANSPARENT) {
    ColorRgb *rgb = &colorRgbs[currentColorIndex];
    md_drawRect(x, y, w, h, rgb->r, rgb->g, rgb->b);
  }
}

void addHitBox(HitBox hb) {
  if (hitBoxesIndex < MAX_HIT_BOX_COUNT) {
    hitBoxes[hitBoxesIndex] = hb;
    hitBoxesIndex++;
  } else {
    consoleLog("too many hit boxes");
  }
}

void endAddingRects() {
  for (int i = 0; i < drawingHitBoxesIndex; i++) {
    addHitBox(drawingHitBoxes[i]);
  }
}

Collision rect(float x, float y, float w, float h) {
  Collision hitCollision;
  initCollision(&hitCollision);
  beginAddingRects();
  addRect(false, x, y, w, h, &hitCollision);
  endAddingRects();
  return hitCollision;
}

Collision box(float x, float y, float w, float h) {
  Collision hitCollision;
  initCollision(&hitCollision);
  beginAddingRects();
  addRect(true, x, y, w, h, &hitCollision);
  endAddingRects();
  return hitCollision;
}

void drawLine(float x, float y, float ox, float oy, Collision *hitCollision) {
  int t = floor(thickness);
  if (t < 3) {
    t = 3;
  } else if (t > 10) {
    t = 10;
  }
  float lx = fabs(ox);
  float ly = fabs(oy);
  float rn = ceil(lx > ly ? lx / t : ly / t);
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

Collision line(float x1, float y1, float x2, float y2) {
  Collision hitCollision;
  initCollision(&hitCollision);
  beginAddingRects();
  drawLine(x1, y1, x2 - x1, y2 - y1, &hitCollision);
  endAddingRects();
  return hitCollision;
}

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
  if (ao < 0.01) {
    return hitCollision;
  }
  if (ao < 0) {
    ao = 0;
  } else if (ao > M_PI * 2) {
    ao = M_PI * 2;
  }
  int lc = ceil(ao * sqrt(radius * 0.25));
  if (lc < 1) {
    lc = 1;
  } else if (lc > 36) {
    lc = 36;
  }
  float ai = ao / lc;
  float a = af;
  float p1x = radius * cos(a) + centerX;
  float p1y = radius * sin(a) + centerY;
  float p2x, p2y;
  float ox, oy;
  for (int i = 0; i < lc; i++) {
    a += ai;
    p2x = radius * cos(a) + centerX;
    p2y = radius * sin(a) + centerY;
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
typedef struct {
  unsigned char grid[CHARACTER_HEIGHT][CHARACTER_WIDTH][3];
  HitBox hitBox;
  int hash;
} CharacterPattern;

#define MAX_CACHED_CHARACTER_PATTERN_COUNT 128
CharacterPattern characterPatterns[MAX_CACHED_CHARACTER_PATTERN_COUNT];
int characterPatternsCount;

void initCharacterPattern() { characterPatternsCount = 0; }

char *colorGridChars = "wrgybpclRGYBPCL";

void setColorGrid(
    char grid[CHARACTER_HEIGHT][CHARACTER_WIDTH + 1],
    unsigned char colorGrid[CHARACTER_HEIGHT][CHARACTER_WIDTH][3]) {
  for (int y = 0; y < CHARACTER_HEIGHT; y++) {
    for (int x = 0; x < CHARACTER_WIDTH; x++) {
      char *ci = strchr(colorGridChars, grid[y][x]);
      if (ci == NULL) {
        colorGrid[y][x][0] = colorGrid[y][x][1] = colorGrid[y][x][2] = 0;
      } else {
        ColorRgb *rgb = &colorRgbs[ci - colorGridChars];
        colorGrid[y][x][0] = rgb->r;
        colorGrid[y][x][1] = rgb->g;
        colorGrid[y][x][2] = rgb->b;
      }
    }
  }
}

void setCharacterHitBox(
    unsigned char grid[CHARACTER_HEIGHT][CHARACTER_WIDTH][3], HitBox *hb) {
  hb->pos.x = -CHARACTER_WIDTH / 2;
  hb->pos.y = -CHARACTER_HEIGHT / 2;
  hb->size.x = CHARACTER_WIDTH;
  hb->size.y = CHARACTER_HEIGHT;
}

void drawCharacter(int index, float x, float y, bool hasCollision, bool isText,
                   Collision *hitCollision) {
  if ((isText && (index < '!' || index > '~')) ||
      (!isText && (index < 'a' || index > 'z'))) {
    return;
  }
  char hashStr[99];
  snprintf(hashStr, 98, "%d:%d", index, isText);
  int hash = getHashFromString(hashStr);
  CharacterPattern *cp = NULL;
  for (int i = 0; i < characterPatternsCount; i++) {
    if (characterPatterns[i].hash == hash) {
      cp = &characterPatterns[i];
      break;
    }
  }
  if (cp == NULL) {
    if (characterPatternsCount >= MAX_CACHED_CHARACTER_PATTERN_COUNT) {
      consoleLog("too many charater patterns");
      return;
    }
    cp = &characterPatterns[characterPatternsCount];
    cp->hash = hash;
    setColorGrid(isText ? textPatterns[index - '!'] : characters[index - 'a'],
                 cp->grid);
    setCharacterHitBox(cp->grid, &cp->hitBox);
    characterPatternsCount++;
  }
  md_drawCharacter(cp->grid, x, y, hash);
  if (hasCollision) {
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
    addHitBox(hb);
  }
}

Collision drawCharacters(char *msg, float x, float y, bool hasCollision,
                         bool isText) {
  Collision hitCollision;
  initCollision(&hitCollision);
  int ml = strlen(msg);
  x -= CHARACTER_WIDTH / 2;
  y -= CHARACTER_HEIGHT / 2;
  for (int i = 0; i < ml; i++) {
    drawCharacter(msg[i], x, y, hasCollision, isText, &hitCollision);
    x += 6;
  }
  return hitCollision;
}

Collision text(char *msg, float x, float y) {
  return drawCharacters(msg, x, y, true, true);
}

Collision character(char *msg, float x, float y) {
  return drawCharacters(msg, x, y, true, false);
}

// Color
ColorRgb colorRgbs[COLOR_COUNT];

void color(int index) { currentColorIndex = index; }

ColorRgb whiteRgb;

ColorRgb getRgb(int index, bool isDarkColor) {
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
    cr.r = isDarkColor ? cr.r * 0.5 : whiteRgb.r - (whiteRgb.r - cr.r) * 0.5;
    cr.g = isDarkColor ? cr.g * 0.5 : whiteRgb.g - (whiteRgb.g - cr.g) * 0.5;
    cr.b = isDarkColor ? cr.b * 0.5 : whiteRgb.b - (whiteRgb.b - cr.b) * 0.5;
  }
  if (index == WHITE) {
    whiteRgb = cr;
  }
  return cr;
}

void initColor() {
  bool isDarkColor = false;
  for (int i = 0; i < COLOR_COUNT; i++) {
    colorRgbs[i] = getRgb(i, isDarkColor);
  }
  if (isDarkColor) {
    colorRgbs[WHITE].r = colorRgbs[BLUE].r * 0.15;
    colorRgbs[WHITE].g = colorRgbs[BLUE].g * 0.15;
    colorRgbs[WHITE].b = colorRgbs[BLUE].b * 0.15;
  }
  color(BLACK);
}

void clearView() {
  md_clearView(colorRgbs[WHITE].r, colorRgbs[WHITE].g, colorRgbs[WHITE].b);
}

// Sound
void play(int type) { playSoundEffect(type); }

void enableSound() { isSoundEnabled = true; }

void disableSound() {
  isSoundEnabled = false;
  md_stopTone();
}

void toggleSound() {
  if (isSoundEnabled) {
    disableSound();
  } else {
    enableSound();
  }
}

// Score
float score;
int prevScore;
int hiScore;

typedef struct {
  char str[9];
  Vector pos;
  float vy;
  int ticks;
} ScoreBoard;

#define MAX_SCORE_BOARD_COUNT 16
ScoreBoard scoreBoards[MAX_SCORE_BOARD_COUNT];
int scoreBoardsIndex;

void initScore() { score = prevScore = hiScore = 0; }

void initScoreBoards() {
  for (int i = 0; i < MAX_SCORE_BOARD_COUNT; i++) {
    scoreBoards[i].ticks = 0;
  }
  scoreBoardsIndex = 0;
}

void updateScoreBoards() {
  for (int i = 0; i < MAX_SCORE_BOARD_COUNT; i++) {
    ScoreBoard *sb = &scoreBoards[i];
    if (sb->ticks > 0) {
      drawCharacters(sb->str, sb->pos.x, sb->pos.y, false, true);
      sb->pos.y += sb->vy;
      sb->vy *= 0.9;
      sb->ticks--;
    }
  }
}

void addScore(float value, float x, float y) {
  score += value;
  ScoreBoard *sb = &scoreBoards[scoreBoardsIndex];
  sprintf(sb->str, "+%d", (int)value);
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

void drawScore() {
  char sc[16];
  int s = state == STATE_IN_GAME ? (int)score : prevScore;
  snprintf(sc, 15, "%d", s);
  drawCharacters(sc, 3, 3, false, true);
  snprintf(sc, 15, "HI %d", hiScore);
  drawCharacters(sc, options.viewSizeX - strlen(sc) * 6 + 2, 3, false, true);
}

void particle(float x, float y, float count, float speed, float angle,
              float angleWidth) {
  addParticle(x, y, count, speed, angle, angleWidth);
}

// Utilities
float rnd(float low, float high) { return getRandom(&gameRandom, low, high); }
int rndi(int low, int high) { return getIntRandom(&gameRandom, low, high); }
void consoleLog(char *format, ...) {
  char cc[99];
  va_list args;
  va_start(args, format);
  vsnprintf(cc, 98, format, args);
  md_consoleLog(cc);
}

float clamp(float v, float low, float high) { return fmax(low, fmin(v, high)); }

float wrap(float v, float low, float high) {
  float w = high - low;
  float o = v - low;
  if (o >= 0) {
    return fmod(o, w) + low;
  } else {
    float wv = w + fmod(o, w) + low;
    if (wv >= high) {
      wv -= w;
    }
    return wv;
  }
}

int getHashFromString(char *str) {
  int hash = 0;
  int len = strlen(str);
  for (int i = 0; i < len; i++) {
    int chr = str[i];
    hash = (hash << 5) - hash + chr;
    hash |= 0;
  }
  return hash;
}

// In game
void initInGame() {
  state = STATE_IN_GAME;
  if (prevScore > hiScore) {
    hiScore = prevScore;
  }
  score = 0;
  initScoreBoards();
  isPlayingBgm = true;
  ticks = -1;
}

// Title
#define MAX_DESCRIPTION_LINE_COUNT 5
char descriptions[MAX_DESCRIPTION_LINE_COUNT][32];
int descriptionLineCount;
int descriptionX;

void parseDescription() {
  descriptionLineCount = 0;
  int dl = 0;
  char *line = strtok(description, "\n");
  while (line != NULL) {
    strncpy(descriptions[descriptionLineCount], line, 31);
    int ll = strlen(line);
    if (ll > dl) {
      dl = ll;
    }
    descriptionLineCount++;
    if (descriptionLineCount >= MAX_DESCRIPTION_LINE_COUNT) {
      break;
    }
    line = strtok(NULL, "\n");
  };
  descriptionX = (options.viewSizeX - dl * CHARACTER_WIDTH) / 2;
}

void initTitle() {
  state = STATE_TITLE;
  ticks = -1;
}

void updateTitle() {
  if (input.isJustPressed) {
    initInGame();
  }
  drawCharacters(title,
                 (options.viewSizeX - strlen(title) * CHARACTER_WIDTH) / 2,
                 options.viewSizeY * 0.25, false, true);
  if (ticks > 30) {
    for (int i = 0; i < descriptionLineCount; i++) {
      drawCharacters(descriptions[i], descriptionX,
                     options.viewSizeY * 0.55 + i * CHARACTER_HEIGHT, false,
                     true);
    }
  }
}

// Game over
int gameOverTicks;
char *gameOverText = "GAME OVER";

void initGameOver() {
  state = STATE_GAME_OVER;
  isPlayingBgm = false;
  prevScore = (int)score;
  gameOverTicks = 0;
  drawCharacters(
      gameOverText,
      (options.viewSizeX - strlen(gameOverText) * CHARACTER_WIDTH) / 2,
      options.viewSizeY * 0.5, false, true);
}

void updateGameOver() {
  if (!hasTitle) {
    if (input.isJustPressed) {
      initInGame();
    }
    return;
  }
  if (gameOverTicks > 20 && input.isJustPressed) {
    initInGame();
  } else if (gameOverTicks > 300) {
    initTitle();
  }
  gameOverTicks++;
}

void end() { initGameOver(); }

// Initialize
EMSCRIPTEN_KEEPALIVE
void initGame() {
  initColor();
  initCharacterPattern();
  initScore();
  initParticle();
  initSound();
  parseDescription();
  setRandomSeedWithTime(&gameRandom);
  hasTitle = strlen(title) + strlen(description) > 0;
  if (hasTitle) {
    initTitle();
  } else {
    initInGame();
  }
}

EMSCRIPTEN_KEEPALIVE
void updateFrame() {
  hitBoxesIndex = 0;
  difficulty = (float)ticks / 60 / FPS + 1;
  if (state == STATE_TITLE) {
    clearView();
    updateTitle();
  } else if (state == STATE_IN_GAME) {
    clearView();
    update();
    updateParticles();
    updateScoreBoards();
  } else if (state == STATE_GAME_OVER) {
    updateGameOver();
  }
  updateSound();
  drawScore();
  ticks++;
}

EMSCRIPTEN_KEEPALIVE
void setInput(bool isPressed, bool isJustPressed, bool isJustReleased) {
  input.isPressed = isPressed;
  input.isJustPressed = isJustPressed;
  input.isJustReleased = isJustReleased;
}
