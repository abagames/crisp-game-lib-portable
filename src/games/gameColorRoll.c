#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "cglp.h"

static char *title = "COLOR ROLL";
static char *description = "[Tap] Shoot";

#define CS static char characters[][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1]
CS = {};
static int charactersCount = 0;

static Options options = {
    .viewSizeX = 100, .viewSizeY = 100, .soundSeed = 5, .isDarkColor = false};

typedef struct {
  float width;
  int color;
} Bar;

typedef struct {
  float x;
  float y;
  float vx;
  Bar bars[8];
  int barCount;
} Lane;
#define COLOR_ROLL_MAX_LANE_COUNT 32
static Lane lanes[COLOR_ROLL_MAX_LANE_COUNT];
static int laneIndex;
static float laneY;
static float shotY;
static int hitColor;
static int laneCount;
static int baseMultiplier;
static int multiplier;
static float penalty;
#define COLOR_ROLL_BAR_COLOR_COUNT 4
static int barColors[COLOR_ROLL_BAR_COLOR_COUNT] = {RED, GREEN, BLUE, YELLOW};
static float laneHeight = 7;

static int addBars(Bar bars[], Bar prevBars[], int prevBarsCount) {
  int cs[9];
  int csCount;
  if (prevBarsCount == 0) {
    cs[0] = barColors[rndi(0, COLOR_ROLL_BAR_COLOR_COUNT)];
    csCount = 1;
  } else {
    TIMES(prevBarsCount, i) { cs[i] = prevBars[i].color; }
    csCount = prevBarsCount;
  }
  if (csCount == 1 || (csCount < 4 && rnd(0, 1) < 0.5)) {
    cs[csCount] = barColors[rndi(0, COLOR_ROLL_BAR_COLOR_COUNT)];
    csCount++;
  } else {
    int ri = rndi(0, csCount);
    csCount--;
    for (int i = ri; i < csCount; i++) {
      cs[i] = cs[i + 1];
    }
  }
  float lx = 99;
  float x = rnd(0, 99);
  TIMES(csCount, i) {
    ASSIGN_ARRAY_ITEM(bars, i, Bar, b);
    float width = i == csCount - 1 ? lx : (99 / csCount) * rnd(0.8, 1.2);
    lx -= width;
    x = wrap(x + width, 0, 99);
    b->width = width;
    b->color = cs[i];
  }
  return csCount;
}

static Bar *prevBars;
static int prevBarsCount;

static void addLane() {
  play(SELECT);
  float x = rnd(0, 99);
  float vx = rnd(0.5, 1) * (rndi(0, 2) * 2 - 1) * sqrt(difficulty);
  ASSIGN_ARRAY_ITEM(lanes, laneIndex, Lane, l);
  l->x = x;
  l->vx = vx;
  if (laneIndex == 0) {
    laneY = 0;
    l->y = 0;
    prevBarsCount = l->barCount = addBars(l->bars, NULL, 0);
  } else {
    l->y = -laneIndex * laneHeight;
    prevBarsCount = l->barCount = addBars(l->bars, prevBars, prevBarsCount);
  }
  prevBars = l->bars;
  laneIndex++;
}

static void update() {
  if (!ticks) {
    laneIndex = 0;
    shotY = -999;
    hitColor = -999;
    laneCount = 2;
    TIMES(laneCount, i) { addLane(); }
    baseMultiplier = 0;
    multiplier = 1;
    penalty = 1;
  }
  float sy = 97;
  if (shotY != -999) {
    shotY -= sqrt(difficulty) * 3;
    sy = shotY;
  } else {
    hitColor = -999;
    if (input.isJustPressed) {
      play(LASER);
      multiplier = 1;
      shotY = sy;
      laneY += 2 * penalty * sqrtf(sqrtf(difficulty));
    }
  }
  color = hitColor == -999 ? BLACK : hitColor;
  rect(49, sy, 3, 99 - sy);
  float my = laneHeight * laneCount;
  laneY += sqrtf(difficulty) * 0.005;
  if (laneY < my) {
    laneY += (my - laneY) * 0.2;
  }
  float ly = laneY;
  float maxY = 0;
  int li = 0;
  TIMES(laneIndex, i) {
    if (li != i) {
      lanes[li] = lanes[i];
    }
    ASSIGN_ARRAY_ITEM(lanes, li, Lane, l);
    l->x = wrap(l->x + l->vx, 0, 99);
    l->y += (ly - l->y) * 0.2;
    float x = l->x;
    bool isRemoved = false;
    bool isShotRemoved = false;
    TIMES(l->barCount, j) {
      ASSIGN_ARRAY_ITEM(l->bars, j, Bar, b);
      color = b->color;
      bool *c;
      if (x + b->width < 99) {
        c = rect(x, l->y, b->width - 1, -laneHeight + 1).isColliding.rect;
      } else {
        c = rect(x, l->y, 99 - x, -laneHeight + 1).isColliding.rect;
        bool *c2 = rect(0, l->y, b->width - (99 - x) - 1, -laneHeight + 1)
                       .isColliding.rect;
        c[BLACK] |= c2[BLACK];
        TIMES(COLOR_ROLL_BAR_COLOR_COUNT, k) {
          int ci = barColors[k];
          c[ci] |= c2[ci];
        }
      }
      if (c[BLACK]) {
        hitColor = b->color;
        isRemoved = true;
      } else if (hitColor != -999) {
        if (c[b->color]) {
          isRemoved = true;
        } else if (c[RED] || c[GREEN] || c[BLUE] || c[YELLOW]) {
          isShotRemoved = true;
        }
      }
      x = wrap(x + b->width, 0, 99);
    }
    ly -= laneHeight;
    if (isShotRemoved) {
      play(HIT);
      shotY = -999;
      penalty = clamp(penalty * (3 / multiplier), 1, 4);
    } else if (isRemoved) {
      play(COIN);
      addScore(multiplier * pow(2, baseMultiplier), 50, l->y);
      laneY -= multiplier;
      multiplier *= 2;
      li--;
    }
    li++;
    if (l->y > maxY) {
      maxY = l->y;
    }
  }
  laneIndex = li;
  if (laneIndex == 0) {
    play(POWER_UP);
    shotY = -999;
    laneCount++;
    if (laneCount > clamp(5 + baseMultiplier, 1, 10)) {
      baseMultiplier = clamp(baseMultiplier + 1, 1, 9);
      laneCount = 2;
    }
  }
  if (shotY == -999) {
    TIMES(laneCount - laneIndex, i) { addLane(); }
    if (maxY > 97) {
      play(EXPLOSION);
      gameOver();
    }
  }
}

void addGameColorRoll() {
  addGame(title, description, characters, charactersCount, options, update);
}
