#include <math.h>
#include <stdbool.h>
#include <stddef.h>

#include "cglp.h"

char *title = "SURVIVOR";
char *description = "[Tap] Jump";

#define CS char characters[][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1]

CS = {{
          "llllll",
          "ll l l",
          "ll l l",
          "llllll",
          " l  ll",
          " l    ",
      },
      {
          "llllll",
          "ll l l",
          "ll l l",
          "llllll",
          "ll  l ",
          "    l ",
      },
      {
          " l  l ",
          " l  l ",
          "llllll",
          "ll l l",
          "ll l l",
          "llllll",
      }};
int charactersCount = 3;

typedef struct _Player {
  Vector pos;
  Vector vel;
  bool isOnFloor;
  bool isJumping;
  bool isJumped;
  struct _Player *underFoot;
  struct _Player *onHead;
  int ticks;
  bool isAlive;
} Player;

typedef struct {
  Vector pos;
  Vector vel;
  bool isAlive;
} DownedPlayer;

typedef struct {
  Vector pos;
  float vx;
  float r;
  float angle;
  bool isAlive;
} Barrel;

#define PLAYER_COUNT 9
Player players[PLAYER_COUNT];
DownedPlayer downedPlayers[PLAYER_COUNT];

Barrel barrel;

void initPlayers() {
  FOR_EACH(players, i) {
    players[i].isAlive = false;
    downedPlayers[i].isAlive = false;
  }
}

void addPlayer() {
  FOR_EACH(players, i) {
    ASSIGN_ARRAY_ITEM(players, i, Player, pl);
    if (!pl->isAlive) {
      vectorSet(&pl->pos, rnd(10, 40), rnd(-9, 9));
      pl->isOnFloor = false;
      pl->isJumping = false;
      pl->isJumped = false;
      pl->underFoot = NULL;
      pl->onHead = NULL;
      pl->ticks = rndi(0, 60);
      pl->isAlive = true;
      break;
    }
  }
}

void addPlayers() {
  play(POWER_UP);
  FOR_EACH(players, i) { addPlayer(); }
}

void addDownedPlayer(Vector pos, float vx, float vy) {
  FOR_EACH(downedPlayers, i) {
    ASSIGN_ARRAY_ITEM(downedPlayers, i, DownedPlayer, dp);
    if (!dp->isAlive) {
      dp->pos = pos;
      vectorSet(&dp->vel, vx, vy);
      dp->isAlive = true;
      break;
    }
  }
}

void update() {
  if (!ticks) {
    initPlayers();
    barrel.isAlive = false;
  }
  if (!barrel.isAlive) {
    addPlayers();
    float r = rnd(5, 25);
    vectorSet(&barrel.pos, 120 + r, 93 - r);
    barrel.vx = rnd(1, 2) / sqrt(r * 0.3 + 1);
    barrel.r = r;
    barrel.angle = rnd(0, M_PI * 2);
    barrel.isAlive = true;
  }
  barrel.pos.x -= barrel.vx * difficulty;
  thickness = 3 + barrel.r * 0.1;
  arc(barrel.pos.x, barrel.pos.y, barrel.r, barrel.angle,
      barrel.angle + M_PI * 2);
  barrel.angle -= barrel.vx / barrel.r;
  particle(barrel.pos.x, barrel.pos.y + barrel.r, barrel.r * 0.05,
           barrel.vx * 5, -0.1, 0.2);
  rect(0, 93, 100, 7);
  int addingPlayerCount = 0;
  FOR_EACH(players, i) {
    ASSIGN_ARRAY_ITEM(players, i, Player, p);
    SKIP_IS_NOT_ALIVE(p);
    p->ticks++;
    if (p->underFoot == NULL) {
      FOR_EACH(players, j) {
        ASSIGN_ARRAY_ITEM(players, j, Player, ap);
        SKIP_IS_NOT_ALIVE(ap);
        if (i != j && p->isOnFloor &&
            distanceTo(&p->pos, ap->pos.x, ap->pos.y) < 4) {
          play(SELECT);
          Player *bp = p;
          for (int k = 0; k < 99; k++) {
            if (bp->underFoot == NULL) {
              break;
            }
            bp = bp->underFoot;
          }
          Player *tp = ap;
          for (int k = 0; k < 99; k++) {
            if (tp->onHead == NULL) {
              break;
            }
            tp = tp->onHead;
          }
          tp->onHead = bp;
          bp->underFoot = tp;
          Player *rp = p;
          for (int k = 0; k < 99; k++) {
            rp->isJumped = rp->isOnFloor = false;
            if (rp->onHead == NULL) {
              break;
            }
            rp = rp->onHead;
          }
          rp = p;
          for (int k = 0; k < 99; k++) {
            rp->isJumped = rp->isOnFloor = false;
            if (rp->underFoot == NULL) {
              break;
            }
            rp = rp->underFoot;
          }
        }
      }
    }
    if (input.isJustPressed &&
        (p->isOnFloor || (p->underFoot != NULL && p->underFoot->isJumped))) {
      play(JUMP);
      vectorSet(&p->vel, 0, -1.5);
      particle(p->pos.x, p->pos.y, 10, 2, M_PI_2, 0.5);
      p->isOnFloor = false;
      p->isJumping = true;
      if (p->underFoot != NULL) {
        p->underFoot->onHead = NULL;
        p->underFoot = NULL;
      }
    }
    if (p->underFoot != NULL) {
      p->pos = p->underFoot->pos;
      p->pos.y -= 6;
      vectorSet(&p->vel, 0, 0);
    } else {
      vectorAdd(&(p->pos), p->vel.x * difficulty, p->vel.y * difficulty);
      p->vel.x *= 0.95;
      if ((p->pos.x < 7 && p->vel.x < 0) || (p->pos.x >= 77 && p->vel.x > 0)) {
        p->vel.x *= -0.5;
      }
      if (p->pos.x < 50) {
        p->vel.x += 0.01 * sqrt(50 - p->pos.x + 1);
      } else {
        p->vel.x -= 0.01 * sqrt(p->pos.x - 50 + 1);
      }
      if (p->isOnFloor) {
        if (p->pos.x < barrel.pos.x) {
          p->vel.x -=
              (0.1 * sqrt(barrel.r)) / sqrt(barrel.pos.x - p->pos.x + 1);
        }
      } else {
        p->vel.y += 0.1;
        if (p->pos.y > 90) {
          p->pos.y = 90;
          p->isOnFloor = true;
          p->isJumped = false;
          p->vel.y = 0;
        }
      }
      if (p->pos.y < 0 && p->vel.y < 0) {
        p->vel.y *= -0.5;
      }
    }
    char pc[2] = {'a' + (p->ticks / 30) % 2, '\0'};
    if (character(pc, p->pos.x, p->pos.y).isColliding.rect[BLACK]) {
      if (p->onHead != NULL) {
        p->onHead->underFoot = NULL;
        p->onHead->isJumping = true;
      }
      if (p->underFoot != NULL) {
        p->underFoot->onHead = NULL;
      }
      play(HIT);
      addDownedPlayer(p->pos, p->vel.x - barrel.vx * 2, p->vel.y);
      p->isAlive = false;
      continue;
    }
    if (p->pos.x < 0 || p->pos.x > 100 || p->pos.y < -50 || p->pos.y > 100) {
      addingPlayerCount++;
      p->isAlive = false;
      continue;
    }
  }
  TIMES(addingPlayerCount, i) { addPlayer(); }
  FOR_EACH(players, i) {
    ASSIGN_ARRAY_ITEM(players, i, Player, p);
    SKIP_IS_NOT_ALIVE(p);
    if (p->isJumping) {
      p->isJumped = true;
      p->isJumping = false;
    }
  }
  FOR_EACH(downedPlayers, i) {
    ASSIGN_ARRAY_ITEM(downedPlayers, i, DownedPlayer, p);
    SKIP_IS_NOT_ALIVE(p);
    p->pos.x += p->vel.x;
    p->pos.y += p->vel.y;
    p->vel.y += 0.2;
    character("c", p->pos.x, p->pos.y);
    p->isAlive = p->pos.y < 105;
  }
  COUNT_IS_ALIVE(players, playerCount);
  if (playerCount == 0) {
    play(RANDOM);
    end();
  }
  if (barrel.pos.x < -barrel.r) {
    barrel.isAlive = false;
    addScore(playerCount, 10, 50);
  }
}
