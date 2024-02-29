#include "cglp.h"

static char *title = "HEXMIN";
static char *description = "[Tap]   Roll\n[Arrow] Move";

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
    .viewSizeX = 100, .viewSizeY = 100, .soundSeed = 9, .isDarkColor = false};

#define HEXMIN_LANE_COUNT 6
static int lanes[HEXMIN_LANE_COUNT];
static float laneChangeTicks;
static float nextLaneAddingCount;
static int arrowAngle;
static float multiplier;
static float ss[6] = {4, 6, 8, 10, 10, 10};
static float rs[5] = {49, 42, 33, 22, 11};
static float keyRepeatTicks;

static void drawHex(Vector *cp, float s) {
  Vector p1;
  Vector p2;
  vectorSet(&p1, cp->x, cp->y);
  addWithAngle(&p1, (5.5 * M_PI) / 3, s);
  for (int i = 0; i < HEXMIN_LANE_COUNT; i++) {
    vectorSet(&p2, p1.x, p1.y);
    addWithAngle(&p1, ((i + 0.5) * M_PI) / 3, s);
    line(p1.x, p1.y, p2.x, p2.y);
  }
}

static void update() {
  if (!ticks) {
    hasCollision = false;
    for (int i = 0; i < HEXMIN_LANE_COUNT; i++) {
      lanes[i] = 0;
    }
    laneChangeTicks = 0;
    nextLaneAddingCount = 0;
    arrowAngle = 0;
    multiplier = 1;
    keyRepeatTicks = 0;
  }
  if (input.isJustPressed || (keyRepeatTicks < 0 && input.isPressed)) {
    keyRepeatTicks = 15 / sqrt(difficulty);
    play(LASER);
    int oa = input.left.isPressed ? -1 : 1;
    arrowAngle = wrap(arrowAngle + oa, 0, 6);
    multiplier = ceil(multiplier * 0.9) - 1;
    if (multiplier < 1) {
      multiplier = 1;
    }
  }
  keyRepeatTicks--;
  Vector p;
  for (int i = 0; i < HEXMIN_LANE_COUNT; i++) {
    int oa = wrap(i - arrowAngle, 0, 6);
    vectorSet(&p, 50, 50);
    addWithAngle(&p, (i * M_PI) / 3, rs[4]);
    color = oa == 0 || oa == 2 ? RED : LIGHT_BLACK;
    thickness = oa == 0 || oa == 2 ? 3 : 1;
    drawHex(&p, ss[4] / 2);
  }
  laneChangeTicks--;
  if (laneChangeTicks < 0) {
    for (int i = 0; i < HEXMIN_LANE_COUNT; i++) {
      if (lanes[i] > 0) {
        play(HIT);
        lanes[i]++;
        if (lanes[i] > 4) {
          play(EXPLOSION);
          gameOver();
        }
      }
    }
    nextLaneAddingCount--;
    if (nextLaneAddingCount < 0) {
      int li = rndi(0, 6);
      for (int i = 0; i < 6; i++) {
        if (lanes[li] == 0) {
          lanes[li] = 1;
          break;
        }
        li = wrap(li + 1, 0, 6);
      }
      nextLaneAddingCount = floor(rnd(0, 1 + 3 / difficulty));
    }
    laneChangeTicks += 30 / sqrt(difficulty);
  }
  for (int j = 0; j < HEXMIN_LANE_COUNT; j++) {
    for (int i = 0; i < 5; i++) {
      if (lanes[j] <= i) {
        break;
      }
      vectorSet(&p, 50, 50);
      addWithAngle(&p, (j * M_PI) / 3, rs[i]);
      color = i < 3 ? GREEN : BLUE;
      thickness = i < 3 ? 2 : 3;
      drawHex(&p, ss[i] / 2);
    }
    if (lanes[j] == 4) {
      int oa = wrap(j - arrowAngle, 0, 6);
      if (oa == 0 || oa == 2) {
        play(COIN);
        addScore((int)multiplier, 50, 50);
        multiplier += 6;
        lanes[j] = 0;
      }
    }
  }
  color = BLACK;
  text("x", 3, 9);
  text(intToChar(multiplier), 9, 9);
}

void addGameHexmin() {
  addGame(title, description, characters, charactersCount, options, update);
}
