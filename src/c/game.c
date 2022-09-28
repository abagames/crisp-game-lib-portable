#include <math.h>
#include <stdbool.h>

#include "cglp.h"

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
  for (int i = 0; i < PLAYER_COUNT; i++) {
    players[i].isAlive = false;
    downedPlayers[i].isAlive = false;
  }
}

void addPlayer() {
  for (int i = 0; i < PLAYER_COUNT; i++) {
    Player *pl = &players[i];
    if (!pl->isAlive) {
      pl->pos.x = rnd(10, 40);
      pl->pos.y = rnd(-9, 9);
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
  for (int i = 0; i < PLAYER_COUNT; i++) {
    addPlayer();
  }
}

void addDownedPlayer(Vector pos, float vx, float vy) {
  for (int i = 0; i < PLAYER_COUNT; i++) {
    DownedPlayer *dp = &downedPlayers[i];
    if (!dp->isAlive) {
      dp->pos = pos;
      dp->vel.x = vx;
      dp->vel.y = vy;
      dp->isAlive = true;
      break;
    }
  }
}

float calcDistance(Vector p1, Vector p2) {
  float ox = p1.x - p2.x;
  float oy = p1.y - p2.y;
  return sqrt(ox * ox + oy * oy);
}

void update() {
  if (!ticks) {
    initPlayers();
    barrel.isAlive = false;
  }
  if (!barrel.isAlive) {
    addPlayers();
    float r = rnd(5, 25);
    barrel.pos.x = 120 + r;
    barrel.pos.y = 93 - r;
    barrel.vx = rnd(1, 2) / sqrt(r * 0.3 + 1);
    barrel.r = r;
    barrel.angle = rnd(0, M_PI * 2);
    barrel.isAlive = true;
  }
  barrel.pos.x -= barrel.vx;  // * difficulty
  thickness = 3 + barrel.r * 0.1;
  arc(barrel.pos.x, barrel.pos.y, barrel.r, barrel.angle,
      barrel.angle + M_PI * 2);
  barrel.angle -= barrel.vx / barrel.r;
  rect(0, 93, 100, 7);
  int addingPlayerCount = 0;
  for (int i = 0; i < PLAYER_COUNT; i++) {
    Player *p = &players[i];
    if (!p->isAlive) {
      continue;
    }
    p->ticks++;
    if (p->underFoot == NULL) {
      for (int j = 0; j < PLAYER_COUNT; j++) {
        Player *ap = &players[j];
        if (!ap->isAlive) {
          continue;
        }
        if (i != j && p->isOnFloor && calcDistance(p->pos, ap->pos) < 4) {
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
      p->vel.x = 0;
      p->vel.y = -1.5;
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
      p->vel.x = p->vel.y = 0;
    } else {
      p->pos.x += p->vel.x;  // * difficulty
      p->pos.y += p->vel.y;  // * difficulty
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
  for (int i = 0; i < addingPlayerCount; i++) {
    addPlayer();
  }
  for (int i = 0; i < PLAYER_COUNT; i++) {
    Player *p = &players[i];
    if (p->isAlive && p->isJumping) {
      p->isJumped = true;
      p->isJumping = false;
    }
  }
  for (int i = 0; i < PLAYER_COUNT; i++) {
    DownedPlayer *p = &downedPlayers[i];
    if (!p->isAlive) {
      continue;
    }
    p->pos.x += p->vel.x;
    p->pos.y += p->vel.y;
    p->vel.y += 0.2;
    character("c", p->pos.x, p->pos.y);
    p->isAlive = p->pos.y < 105;
  }
  if (barrel.pos.x < -barrel.r) {
    barrel.isAlive = false;
  }
}
