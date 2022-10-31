#include "cglp.h"

static char *title = "FROOOOG";
static char *description = "[Hold]    Bend\n[Release] Jump";

#define CS static char characters[][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1]
CS = {{
    "      ",
    "      ",
    "      ",
    "      ",
    "      ",
    "      ",
}};
static int charactersCount = 1;

static Options options = {
    .viewSizeX = 100, .viewSizeY = 100, .soundSeed = 4, .isDarkColor = false};

typedef struct {
  float y;
  float vx;
  float width;
  int color;
  float interval;
  float ticks;
  bool isAlive;
} Lane;
#define FROOOOG_MAX_LANE_COUNT 16
Lane lanes[FROOOOG_MAX_LANE_COUNT];
int laneIndex;
int nextEmptyLaneCount;
typedef struct {
  Vector pos;
  float vx;
  float width;
  int color;
  bool isAlive;
} Car;
#define FROOOOG_MAX_CAR_COUNT 32
Car cars[FROOOOG_MAX_CAR_COUNT];
int carIndex;
float nextLaneY;
typedef struct {
  float y;
  float py;
  float targetY;
  bool isSafe;
  int state;
} Frog;
Frog frog;

static void addLane(bool isEmpty) {
  nextEmptyLaneCount--;
  float vr = sqrtf(difficulty) + 0.1;
  float vx = (rnd(0, vr) * rnd(0, vr) * rnd(0, vr) + 1) * 0.3 * RNDPM();
  float width = rndi(9, 19);
  float interval = width + rnd(40, 90) / fabsf(vx);
  ASSIGN_ARRAY_ITEM(lanes, laneIndex, Lane, l);
  l->y = nextLaneY;
  l->vx = vx;
  l->width = width;
  int cs[] = {RED, PURPLE, YELLOW, BLUE, CYAN};
  l->color = cs[rndi(0, 5)];
  l->interval = interval;
  l->ticks = nextEmptyLaneCount < 0 || isEmpty ? 9999999 : rnd(0, interval / 2);
  l->isAlive = true;
  laneIndex = wrap(laneIndex + 1, 0, FROOOOG_MAX_LANE_COUNT);
  nextLaneY -= 10;
  if (nextEmptyLaneCount < 0) {
    nextEmptyLaneCount = rndi(9, 16);
  }
}

static void updateLanes() {
  FOR_EACH(lanes, i) {
    ASSIGN_ARRAY_ITEM(lanes, i, Lane, l);
    SKIP_IS_NOT_ALIVE(l);
    if (l->ticks > 999) {
      color = LIGHT_GREEN;
      box(50, l->y + 5, 100, 9);
    } else {
      color = LIGHT_BLACK;
      box(50, l->y, 100, 1);
    }
    l->ticks--;
    if (l->ticks < 0) {
      ASSIGN_ARRAY_ITEM(cars, carIndex, Car, c);
      vectorSet(&c->pos, l->vx < 0 ? 99 + l->width / 2 : -l->width / 2,
                l->y + 5);
      c->vx = l->vx;
      c->width = l->width;
      c->color = l->color;
      c->isAlive = true;
      carIndex = wrap(carIndex + 1, 0, FROOOOG_MAX_CAR_COUNT);
      l->ticks = l->interval * rnd(0.9, 1.6);
    }
    l->isAlive = l->y <= 99;
  }
  FOR_EACH(cars, i) {
    ASSIGN_ARRAY_ITEM(cars, i, Car, c);
    SKIP_IS_NOT_ALIVE(c);
    c->pos.x += c->vx;
    color = c->color;
    box(VEC_XY(c->pos), c->width, 7);
    c->isAlive =
        !(c->pos.x < -c->width || c->pos.x > 99 + c->width || c->pos.y > 103);
  }
}

void drawFrog(float scale) {
  float y = frog.y;
  color = GREEN;
  bool *c = box(50, y, 3 * scale, 5 * scale).isColliding.rect;
  if (scale == 1 && (c[RED] || c[YELLOW] || c[PURPLE] || c[BLUE] || c[CYAN])) {
    play(EXPLOSION);
    gameOver();
  }
  float ox = 2 * scale;
  float oy = 2 * scale * scale;
  float w = 2 * scale;
  float h = 3 * scale;
  box(50 - ox, y - oy, w, h);
  box(49 + ox, y - oy, w, h);
  box(50 - ox, y + oy, w, h);
  box(49 + ox, y + oy, w, h);
}

static void update() {
  if (!ticks) {
    INIT_UNALIVED_ARRAY(lanes);
    laneIndex = 0;
    INIT_UNALIVED_ARRAY(cars);
    carIndex = 0;
    nextLaneY = 90;
    nextEmptyLaneCount = 0;
    TIMES(3, i) { addLane(true); }
    TIMES(7, i) { addLane(false); }
    TIMES(99, i) { updateLanes(); }
    frog.y = 95;
    frog.py = 0;
    frog.targetY = 0;
    frog.state = 0;
    frog.isSafe = true;
  }
  float scr = difficulty * 0.02;
  if (frog.y < 90) {
    scr += (90 - frog.y) * 0.1;
  }
  if (frog.y > 103) {
    play(EXPLOSION);
    gameOver();
  }
  FOR_EACH(lanes, i) {
    ASSIGN_ARRAY_ITEM(lanes, i, Lane, l);
    SKIP_IS_NOT_ALIVE(l);
    l->y += scr;
  }
  FOR_EACH(cars, i) {
    ASSIGN_ARRAY_ITEM(cars, i, Car, c);
    SKIP_IS_NOT_ALIVE(c);
    c->pos.y += scr;
  }
  nextLaneY += scr;
  if (nextLaneY > -50) {
    addLane(false);
  }
  frog.y += scr;
  frog.py += scr;
  frog.targetY += scr;
  updateLanes();
  if (frog.state == 0) {
    drawFrog(1);
    if (input.isPressed) {
      play(SELECT);
      frog.state = 1;
      frog.targetY = frog.y - 3;
    }
  }
  if (frog.state == 1) {
    frog.targetY -= sqrt(difficulty) * 0.7;
    color = LIGHT_BLACK;
    rect(49, frog.targetY, 1, frog.y - frog.targetY);
    drawFrog(1);
    float ty = frog.targetY;
    int li = laneIndex;
    TIMES(FROOOOG_MAX_LANE_COUNT, i) {
      li = wrap(li + 1, 0, FROOOOG_MAX_LANE_COUNT);
      ASSIGN_ARRAY_ITEM(lanes, li, Lane, l);
      SKIP_IS_NOT_ALIVE(l);
      if (l->y > frog.targetY) {
        ty = l->y - 5;
      }
    }
    color = LIGHT_BLACK;
    box(50, ty, 3, 5);
    if (input.isJustReleased || frog.targetY < 9) {
      play(POWER_UP);
      frog.state = 2;
      frog.targetY = ty;
      frog.py = frog.y;
    }
  }
  if (frog.state == 2) {
    frog.y -= sqrt(difficulty) * 1.5;
    float scale =
        sinf(((frog.y - frog.targetY) / (frog.py - frog.targetY)) * M_PI) + 1;
    drawFrog(scale);
    if (frog.y < frog.targetY) {
      play(HIT);
      color = TRANSPARENT;
      frog.y = frog.targetY;
      bool isf = box(50, frog.y, 1, 1).isColliding.rect[LIGHT_GREEN];
      int lc = isf || frog.isSafe ? 0 : ceilf((frog.py - frog.y - 1) / 10);
      addScore(lc * lc, 50, frog.y);
      frog.state = 0;
      frog.isSafe = isf;
    }
  }
}

void addGameFroooog() {
  addGame(title, description, characters, charactersCount, options, update);
}
