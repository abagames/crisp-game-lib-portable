#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "cglp.h"

char *title = "BALL TOUR";
char *description = "[Hold]\n Move forward";

#define CS char characters[][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1]
CS = {{
          "llllll",
          "l l ll",
          "l l ll",
          "llllll",
          " l  l ",
          " l  l ",
      },
      {
          "      ",
          "llllll",
          "l l ll",
          "l l ll",
          "llllll",
          "ll  ll",
      },
      {
          "  lll ",
          " ll ll",
          " l lll",
          " lllll",
          "  lll ",
          "  ll  ",
      }};
int charactersCount = 3;

Options options = {
    .viewSizeX = 100, .viewSizeY = 100, .soundSeed = 4, .isDarkColor = true};

typedef struct {
  Vector pos;
  float yAngle;
  float vx;
  float ticks;
  bool isAlive;
} Player;
static Player player;
typedef struct {
  Vector pos;
  float vy;
  bool isAlive;
} Spike;
#define MAX_SPIKE_COUNT 32
static Spike spikes[MAX_SPIKE_COUNT];
static int spikeIndex;
static float nextSpikeDist;
typedef struct {
  Vector pos;
  bool isAlive;
} Ball;
#define MAX_BALL_COUNT 16
static Ball balls[MAX_BALL_COUNT];
static int ballIndex;
static float nextBallDist;
static float multiplier;

void update() {
  if (!ticks) {
    vectorSet(&player.pos, 90, 50);
    player.yAngle = 0;
    player.vx = 0;
    player.ticks = 0;
    INIT_UNALIVED_ARRAY(spikes);
    spikeIndex = 0;
    nextSpikeDist = 0;
    INIT_UNALIVED_ARRAY(balls);
    ballIndex = 0;
    nextBallDist = 9;
    multiplier = 1;
  }
  color = BLUE;
  rect(0, 90, 100, 9);
  nextSpikeDist -= player.vx;
  if (nextSpikeDist < 0) {
    ASSIGN_ARRAY_ITEM(spikes, spikeIndex, Spike, s);
    vectorSet(&s->pos, -9, rnd(10, 80));
    s->vy = rnd(0, 1) < 0.2
                ? rnd(1, difficulty) * (rnd(0, 1) < 0.5 ? -1 : 1) * 0.3
                : 0;
    s->isAlive = true;
    nextSpikeDist += rnd(9, 49);
    spikeIndex = wrap(spikeIndex + 1, 0, MAX_SPIKE_COUNT);
  }
  color = BLACK;
  FOR_EACH(spikes, i) {
    ASSIGN_ARRAY_ITEM(spikes, i, Spike, s);
    SKIP_IS_NOT_ALIVE(s);
    s->pos.x += player.vx;
    s->pos.y += s->vy;
    if (s->pos.y < 10 || s->pos.y > 80) {
      s->vy *= -1;
    }
    text("*", s->pos.x, s->pos.y);
    s->isAlive = s->pos.x <= 103;
  }
  float py = player.pos.y;
  player.yAngle += difficulty * 0.05;
  player.pos.y = sinf(player.yAngle) * 30 + 50;
  player.ticks += clamp((py - player.pos.y) * 9 + 1, 0, 9);
  if (input.isJustPressed) {
    play(HIT);
  }
  player.vx = (input.isPressed ? 1 : 0.1) * difficulty;
  char pc[2] = {'a' + (int)(player.ticks / 50) % 2, '\0'};
  character(pc, player.pos.x, player.pos.y);
  color = RED;
  if (character("c", player.pos.x, player.pos.y - 6).isColliding.text['*']) {
    play(EXPLOSION);
    end();
  }
  color = GREEN;
  FOR_EACH(balls, i) {
    ASSIGN_ARRAY_ITEM(balls, i, Ball, b);
    SKIP_IS_NOT_ALIVE(b);
    b->pos.x += player.vx;
    bool *c = character("c", b->pos.x, b->pos.y).isColliding.character;
    if (c['a'] || c['b'] || c['c']) {
      addScore((int)multiplier, player.pos.x, player.pos.y);
      multiplier += 10;
      play(SELECT);
      b->isAlive = false;
      continue;
    }
    b->isAlive = b->pos.x <= 103;
  }
  nextBallDist -= player.vx;
  if (nextBallDist < 0) {
    Vector p;
    vectorSet(&p, -3, rnd(20, 70));
    color = TRANSPARENT;
    if (character("c", p.x, p.y).isColliding.text['*']) {
      nextBallDist += 9;
    } else {
      ASSIGN_ARRAY_ITEM(balls, ballIndex, Ball, b);
      b->pos = p;
      b->isAlive = true;
      nextBallDist += rnd(25, 64);
      ballIndex = wrap(ballIndex + 1, 0, MAX_BALL_COUNT);
    }
  }
  multiplier = clamp(multiplier - 0.02 * difficulty, 1, 999);
  color = BLACK;
  char mm[9];
  sprintf(mm, "x%d", (int)multiplier);
  text(mm, 3, 9);
}
