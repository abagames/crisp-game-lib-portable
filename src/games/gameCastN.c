#include "cglp.h"

static char *title = "CAST N";
static char *description =
    "[Hold]    Select power\n[Release] Cast\n[Tap]     Pull";

#define CS static char characters[][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1]
CS = {
    {
        "      ",
        "  YYY ",
        " YyyyY",
        " YyyyY",
        " YyyyY",
        "  YYY ",
    },
    {
        "      ",
        "  lll ",
        "l ll l",
        "llllll",
        "l llll",
        "  ll   ",
    },
    {
        "      ",
        "llll  ",
        "llllll",
        "l l   ",
        "   lll",
        "      ",
    },
};
static int charactersCount = 2;

static Options options = {
    .viewSizeX = 150, .viewSizeY = 100, .soundSeed = 2, .isDarkColor = false};

typedef struct _Node {
  Vector pos;
  Vector vel;
  struct _Node *nextNode;
} Node;
#define CAST_N_NODES_COUNT 20
static Node nodes[CAST_N_NODES_COUNT];
// 0:ready, 1:angle, 2:throw, 3:pull
static int nodeState;
static float throwPower;
typedef struct {
  Vector pos;
  Vector vel;
  int type;
  bool isAlive;
} Fish;
#define CAST_N_MAX_FISH_COUNT 80
static Fish fishes[CAST_N_MAX_FISH_COUNT];
static int fishIndex;
static int nextFishTicks;
static int nextRedFishCount;
static float waterY;
static int multiplier;
static Vector startPos;
static float nodeDist = 9;

static void update() {
  if (!ticks) {
    vectorSet(&startPos, 5, 20);
    TIMES(CAST_N_NODES_COUNT, i) {
      Node *n = &nodes[i];
      vectorSet(&n->pos, VEC_XY(startPos));
      vectorSet(&n->vel, 0, 0);
    }
    TIMES(CAST_N_NODES_COUNT - 1, i) { nodes[i + 1].nextNode = &nodes[i]; }
    nodeState = 0;
    throwPower = 0;
    INIT_UNALIVED_ARRAY(fishes);
    fishIndex = 0;
    nextFishTicks = 0;
    nextRedFishCount = 1;
    waterY = 40;
    multiplier = 1;
  }
  color = LIGHT_YELLOW;
  thickness = 3;
  line(0, startPos.y + 3, 150, 120);
  color = LIGHT_BLUE;
  rect(0, waterY, 150, 3);
  color = BLACK;
  character("a", VEC_XY(nodes[0].pos));
  if (nodeState != 3 && waterY < startPos.y - 4) {
    play(EXPLOSION);
    gameOver();
  }
  if (nodeState == 0) {
    if (input.isJustPressed) {
      play(SELECT);
      throwPower = 1;
      nodeState = 1;
      multiplier = 1;
    }
  }
  thickness = 2;
  if (nodeState == 1) {
    throwPower += 0.05 * sqrt(difficulty);
    float a = 0.1 - throwPower * 0.2;
    Vector p;
    addWithAngle(vectorSet(&p, VEC_XY(startPos)), a, throwPower * 5 + 3);
    line(VEC_XY(startPos), VEC_XY(p));
    if (input.isJustReleased || throwPower > 3) {
      play(JUMP);
      throwPower = clamp(throwPower, 1, 3);
      nodeState = 2;
      rotate(vectorSet(&nodes[0].vel, sqrt(difficulty) * throwPower, 0), a);
    }
  }
  if (nodeState == 2) {
    FOR_EACH(nodes, i) {
      ASSIGN_ARRAY_ITEM(nodes, i, Node, n);
      n->pos.x = clamp(n->pos.x, 0, 147);
      if (i == 0) {
        if (!character("a", n->pos.x, n->pos.y)
                 .isColliding.rect[LIGHT_YELLOW]) {
          float py = n->pos.y;
          vectorAdd(&n->pos, n->vel.x, n->vel.y);
          if (py < waterY && n->pos.y >= waterY) {
            n->vel.x = 0;
            n->vel.y *= 0.1;
          }
        }
        n->vel.y += (n->pos.y < waterY ? 0.05 : 0.01) * difficulty;
        vectorMul(&n->vel, 0.99);
      } else {
        if (!line(VEC_XY(n->nextNode->pos), VEC_XY(n->pos))
                 .isColliding.rect[LIGHT_YELLOW]) {
          float d = distanceTo(&n->pos, VEC_XY(n->nextNode->pos));
          if (d > nodeDist) {
            float a = angleTo(&n->nextNode->pos, VEC_XY(n->pos));
            addWithAngle(vectorSet(&n->pos, VEC_XY(n->nextNode->pos)), a,
                         nodeDist);
          }
          vectorAdd(&n->pos, VEC_XY(n->vel));
          n->vel.y += (n->pos.y < waterY ? 0.005 : 0.001) * difficulty;
          vectorMul(&n->vel, 0.99);
        }
      }
    }
    if (input.isJustPressed) {
      play(POWER_UP);
      nodeState = 3;
    }
  }
  if (nodeState == 3) {
    FOR_EACH(nodes, i) {
      ASSIGN_ARRAY_ITEM(nodes, i, Node, n);
      vectorSet(&n->vel, 0, 0);
      if (distanceTo(&n->pos, VEC_XY(startPos)) > 1 && n->pos.x > startPos.x) {
        float a = angleTo(&n->pos, VEC_XY(startPos));
        addWithAngle(&n->pos, a, sqrt(difficulty) * 2);
      } else {
        vectorSet(&n->pos, VEC_XY(startPos));
        if (i == 0) {
          nodeState = 0;
        }
      }
      if (i > 0) {
        line(VEC_XY(n->nextNode->pos), VEC_XY(n->pos));
      }
    }
  }
  COUNT_IS_ALIVE(fishes, fc);
  if (fc == 0) {
    nextFishTicks = 0;
  }
  nextFishTicks--;
  if (nextFishTicks < 0) {
    nextRedFishCount--;
    int type = 1;
    if (nextRedFishCount < 0) {
      type = 0;
      nextRedFishCount = rndi(1, 7);
    }
    int c = rndi(3, 8);
    Vector cp;
    vectorSet(&cp, 153, rnd(waterY + (type == 0 ? 19 : 9), 90));
    float vx = -rnd(1, sqrt(difficulty)) * (type == 0 ? 0.3 : 0.2);
    TIMES(c, i) {
      ASSIGN_ARRAY_ITEM(fishes, fishIndex, Fish, f);
      Vector p;
      vectorAdd(vectorSet(&p, cp.x, cp.y), rnd(0, 20), rnd(0, 9) * RNDPM());
      p.x = clamp(p.x, 153, 180);
      p.y = clamp(p.y, waterY + 4, 96);
      vectorSet(&f->pos, VEC_XY(p));
      vectorSet(&f->vel, vx, 0);
      f->type = type;
      f->isAlive = true;
      fishIndex = wrap(fishIndex + 1, 0, CAST_N_MAX_FISH_COUNT);
    }
    nextFishTicks = rnd(120, 150) / sqrt(difficulty);
  }
  FOR_EACH(fishes, i) {
    ASSIGN_ARRAY_ITEM(fishes, i, Fish, f);
    SKIP_IS_NOT_ALIVE(f);
    color = f->type == 0 ? RED : BLUE;
    characterOptions.isMirrorX = f->vel.x < 0;
    char fc[2] = {'c' - f->type, '\0'};
    Collisions c = character(fc, VEC_XY(f->pos)).isColliding;
    if (c.rect[BLACK] || c.character['a']) {
      if (nodeState == 3) {
        float a = angleTo(&f->pos, VEC_XY(startPos));
        addWithAngle(&f->pos, a, sqrt(difficulty) * 2);
        f->pos.y -= difficulty * 0.3;
        if (f->pos.x < startPos.x + 9) {
          addScore(f->type == 0 ? -1 : multiplier,
                   startPos.x + clamp(multiplier * 6, 0, 140),
                   startPos.y + (f->type == 0 ? 9 : 0));
          multiplier++;
          waterY =
              clamp(waterY + (f->type == 0 ? -5 : 1) * sqrt(difficulty), 0, 50);
          play(f->type == 0 ? HIT : COIN);
          f->isAlive = false;
          continue;
        }
      }
      if (!c.rect[LIGHT_YELLOW]) {
        f->pos.y += difficulty * 0.3;
      }
      if (rnd(0, 1) < 0.1) {
        f->vel.x *= -1;
      }
      continue;
    }
    if (f->pos.y < waterY + 3) {
      f->vel.y += 0.1;
    } else {
      f->vel.y = 0;
    }
    vectorAdd(&f->pos, VEC_XY(f->vel));
    f->vel.x += rnd(0, sqrt(difficulty)) * RNDPM() * 0.01;
    if (c.rect[LIGHT_YELLOW] && f->vel.x < 0) {
      f->vel.x *= -1;
    }
    f->isAlive = !((f->vel.x > 0 && f->pos.x > 153) || f->pos.x < -3);
  }
  characterOptions.isMirrorX = false;
  waterY -= sqrt(difficulty) * 0.01;
}

void addGameCastN() {
  addGame(title, description, characters, charactersCount, options, update);
}
