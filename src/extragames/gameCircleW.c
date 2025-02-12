//note requires at least around 800-820 hitcount set or circles will overlap each other
//default was set to 256
#include "cglp.h"

static char *title = "CIRCLE W";
static char *description = "[Hold] Expand";

static Options options = {
  .viewSizeX = 200, .viewSizeY = 100, 
  .soundSeed = 5, .isDarkColor = false
};

#define CIRCLE_W_MAX_CIRCLE_COUNT 128
typedef struct {
  Vector pos;
  float radius;
  enum { NORMAL, PLAYER, DANGER } type;
  bool isAlive;
} Circle;

static Circle circles[CIRCLE_W_MAX_CIRCLE_COUNT];
static float nextCircleDist;
static int nextDangerCount;
static float playerRadius;
static Vector scr;
static int multiplier;

static void addCircle(Vector pos, float radius, int type) {
  FOR_EACH(circles, i) {
    ASSIGN_ARRAY_ITEM(circles, i, Circle, c);
    if (!c->isAlive) {
      c->pos = pos;
      c->radius = radius;
      c->type = type;
      c->isAlive = true;
      return;
    }
  }
}

static void update() {
  if (!ticks) {
    INIT_UNALIVED_ARRAY(circles);
    Vector initialPos = {200, 50};
    addCircle(initialPos, 10, PLAYER);
    nextCircleDist = 0;
    nextDangerCount = 30;
    playerRadius = 1;
    multiplier = 1;
    scr.x = 0;
    scr.y = 0;
  }
  if (input.isJustPressed) {
    play(LASER);
  }
  if (playerRadius < 9 && multiplier > 1) {
    multiplier--;
  }
  playerRadius += sqrtf(difficulty) * (input.isPressed ? 1 : -1) * 0.5f;
  if (playerRadius < 1) {
    playerRadius = 1;
  }
  Circle *playerCircle = NULL;
  FOR_EACH(circles, i) {
    ASSIGN_ARRAY_ITEM(circles, i, Circle, c);
    SKIP_IS_NOT_ALIVE(c);
    vectorAdd(&c->pos, VEC_XY(scr));
    c->pos.y = wrap(c->pos.y, -15, 115);
    if (c->type == PLAYER) {
      playerCircle = c;
    } else {
      c->isAlive = c->pos.x >= -c->radius;
    }
  }
  color = GREEN;
  arc(VEC_XY(playerCircle->pos), playerRadius, 0, M_PI * 2);
  if (playerCircle->pos.x < 20) {
    playerCircle->pos.x = 20;
  }
  bool isSetPlayer = false;
  FOR_EACH(circles, i) {
    ASSIGN_ARRAY_ITEM(circles, i, Circle, c);
    SKIP_IS_NOT_ALIVE(c);
    if (c->type == PLAYER) {
      continue;
    }
    color = c->type == DANGER ? RED : BLUE;
    if (arc(VEC_XY(c->pos), c->radius, 0, M_PI * 2).isColliding.rect[GREEN]) {
      if (c->type == DANGER) {
        play(EXPLOSION);
        gameOver();
      } else if (!isSetPlayer) {
        play(COIN);
        multiplier += ceilf(playerRadius);
        addScore(multiplier, VEC_XY(c->pos));
        c->type = PLAYER;
        playerRadius = c->radius;
        isSetPlayer = true;
        playerCircle->pos.x = -99;
        playerCircle->type = NORMAL;
      }
    }
  }
  nextCircleDist += scr.x;
  if (nextCircleDist < 0) {
    Vector pos;
    float radius;
    int type = nextDangerCount == 0 ? DANGER : NORMAL;
    color = TRANSPARENT;
    for (int i = 0; i < 9; i++) {
      radius = rnd(8, 15);
      pos.x = 200 + radius;
      pos.y = rnd(0, 99);
      Collision col = box(VEC_XY(pos), radius * (type == DANGER ? 8 : 5), radius * 2.5f);
      if (!(col.isColliding.rect[RED] || col.isColliding.rect[BLUE])) {
        addCircle(pos, radius, type);
        nextDangerCount--;
        if (nextDangerCount < 0) {
          play(HIT);
          nextDangerCount = rndi(24, 30);
        }
        nextCircleDist = 5;
        break;
      }
    }
  }
  vectorSet(&scr, -sqrtf(difficulty), (50 - playerCircle->pos.y) * 0.1f);
  if (playerCircle->pos.x > 20) {
    scr.x -= (playerCircle->pos.x - 20) * 0.1f;
  }
  color = BLACK;
  text("x", 0, 9);
  text(intToChar(multiplier), 5, 9);
}

void addGameCirclew() {
  addGame(title, description, NULL, 0, options, update);
}