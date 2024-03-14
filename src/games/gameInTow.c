#include "cglp.h"

static char* title = "IN TOW";
static char* description = "[Tap] Multiple jumps";

#define CS static char characters[][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1]
CS = {{
          " bbbb ",
          "bbblwb",
          "bbbbyy",
          "  bb  ",
          "bbbb  ",
          " y y  ",
      },
      {
          " bbbb ",
          "bbblwb",
          "bbbbyy",
          "bbbb  ",
          " bbb  ",
          " y y  ",
      },
      {
          "      ",
          "      ",
          " yy   ",
          " yyl  ",
          "yyyy  ",
          " yy   ",
      },
      {
          " rrr l",
          "rrrr l",
          "rrrr l",
          "rrrr l",
          "rrrr l",
          " rrr l",
      }};
static int charactersCount = 4;

static Options options = {
    .viewSizeX = 200, .viewSizeY = 100, .soundSeed = 50, .isDarkColor = false};

typedef struct {
  Vector pos;
  float vy;
  Vector posHistory[100];
  bool isJumping;
} Bird;
static Bird bird;
typedef struct {
  float index;
  int targetIndex;
} Chick;
#define IN_TOW_MAX_CHICK_COUNT 30
static Chick chicks[IN_TOW_MAX_CHICK_COUNT];
static int chickCount;
typedef struct {
  Vector pos;
  float vy;
  bool isAlive;
} FallingChick;
#define IN_TOW_MAX_FALLING_CHICK_COUNT 32
static FallingChick fallingChicks[IN_TOW_MAX_FALLING_CHICK_COUNT];
static int fallingChickIndex;
typedef struct {
  Vector pos;
  float width;
  bool hasChick;
  bool isAlive;
} Floor;
#define IN_TOW_MAX_FLOOR_COUNT 32
static Floor floors[IN_TOW_MAX_FLOOR_COUNT];
static int floorIndex;
static float nextFloorDist;
typedef struct {
  Vector pos;
  float vx;
  bool isAlive;
} Bullet;
#define IN_TOW_MAX_BULLET_COUNT 32
static Bullet bullets[IN_TOW_MAX_BULLET_COUNT];
static int bulletIndex;
static float nextBulletDist;
static bool isFalling;

static void update() {
  if (!ticks) {
    vectorSet(&bird.pos, 64, 32);
    bird.vy = 0;
    bird.isJumping = true;
    chickCount = 0;
    INIT_UNALIVED_ARRAY(fallingChicks);
    fallingChickIndex = 0;
    INIT_UNALIVED_ARRAY(floors);
    ASSIGN_ARRAY_ITEM(floors, floorIndex, Floor, f);
    vectorSet(&f->pos, 70, 70);
    f->width = 90;
    f->hasChick = false;
    f->isAlive = true;
    floorIndex = wrap(floorIndex + 1, 0, IN_TOW_MAX_FLOOR_COUNT);
    ASSIGN_ARRAY_ITEM(floors, floorIndex, Floor, f2);
    vectorSet(&f2->pos, 150, 50);
    f2->width = 90;
    f2->hasChick = true;
    f2->isAlive = true;
    floorIndex = wrap(floorIndex + 1, 0, IN_TOW_MAX_FLOOR_COUNT);
    nextFloorDist = 0;
    INIT_UNALIVED_ARRAY(bullets);
    bulletIndex = 0;
    nextBulletDist = 99;
    isFalling = false;
  }
  float scr = sqrtf(difficulty);
  if (bird.isJumping) {
    if (chickCount > 0 && input.isJustPressed) {
      play(JUMP);
      play(HIT);
      bird.vy = -2 * sqrtf(difficulty);
      chickCount--;
      ASSIGN_ARRAY_ITEM(fallingChicks, fallingChickIndex, FallingChick, fc);
      vectorSet(&fc->pos, bird.posHistory[2].x, bird.posHistory[2].y);
      fc->vy = 0;
      fc->isAlive = true;
      fallingChickIndex =
          wrap(fallingChickIndex + 1, 0, IN_TOW_MAX_FALLING_CHICK_COUNT);
    }
    Vector pp;
    vectorSet(&pp, VEC_XY(bird.pos));
    bird.vy += (input.isPressed ? 0.05 : 0.2) * difficulty;
    bird.pos.y += bird.vy;
    Vector op;
    vectorSet(&op, VEC_XY(bird.pos));
    vectorAdd(&op, -pp.x, -pp.y);
    vectorMul(&op, 1.0 / 9);
    color = WHITE;
    TIMES(9, i) {
      vectorAdd(&pp, VEC_XY(op));
      box(VEC_XY(pp), 6, 6);
    }
  } else {
    if (input.isJustPressed) {
      play(JUMP);
      bird.vy = -2 * sqrtf(difficulty);
      bird.isJumping = true;
    }
  }
  color = BLACK;
  char pc[2] = {bird.vy < 0 ? 'b' : 'a', '\0'};
  character(pc, VEC_XY(bird.pos));
  nextFloorDist -= scr;
  if (nextFloorDist < 0) {
    float width = rnd(40, 80);
    ASSIGN_ARRAY_ITEM(floors, floorIndex, Floor, f);
    vectorSet(&f->pos, 200 + width / 2, rndi(30, 90));
    f->width = width;
    f->hasChick = true;
    f->isAlive = true;
    floorIndex = wrap(floorIndex + 1, 0, IN_TOW_MAX_FLOOR_COUNT);
    nextFloorDist += width + rnd(10, 30);
  }
  FOR_EACH(floors, i) {
    ASSIGN_ARRAY_ITEM(floors, i, Floor, f);
    SKIP_IS_NOT_ALIVE(f);
    f->pos.x -= scr;
    color = LIGHT_YELLOW;
    Collision c = box(VEC_XY(f->pos), f->width, 4);
    if (bird.vy > 0 && c.isColliding.rect[WHITE]) {
      bird.pos.y = f->pos.y - 5;
      bird.isJumping = false;
      bird.vy = 0;
    }
    if (f->hasChick) {
      color = BLACK;
      char* cc = "c";
      Collision cl = character(cc, f->pos.x, f->pos.y - 5);
      if (cl.isColliding.character['a'] || cl.isColliding.character['b']) {
        if (chickCount < 30) {
          Chick* c = &(chicks[chickCount]);
          c->index = 0;
          c->targetIndex = 0;
          chickCount++;
        }
        play(SELECT);
        addScore(chickCount, f->pos.x, f->pos.y - 5);
        f->hasChick = false;
      }
    }
    f->isAlive = f->pos.x >= -f->width / 2;
  }
  for (int i = 98; i >= 0; i--) {
    bird.posHistory[i + 1].x = bird.posHistory[i].x - scr;
    bird.posHistory[i + 1].y = bird.posHistory[i].y;
  }
  bird.posHistory[0].x = bird.pos.x;
  bird.posHistory[0].y = bird.pos.y;
  color = TRANSPARENT;
  if (!bird.isJumping) {
    Collision cl = box(bird.pos.x, bird.pos.y + 4, 9, 2);
    if (!cl.isColliding.rect[LIGHT_YELLOW]) {
      bird.isJumping = true;
    }
  }
  nextBulletDist -= scr;
  if (nextBulletDist < 0) {
    ASSIGN_ARRAY_ITEM(bullets, bulletIndex, Bullet, b);
    vectorSet(&b->pos, 203, rndi(10, 90));
    b->vx = rnd(1, difficulty) * 0.3;
    b->isAlive = true;
    bulletIndex = wrap(bulletIndex + 1, 0, IN_TOW_MAX_BULLET_COUNT);
    nextBulletDist += rnd(50, 80) / sqrtf(difficulty);
  }
  color = BLACK;
  FOR_EACH(bullets, i) {
    ASSIGN_ARRAY_ITEM(bullets, i, Bullet, b);
    SKIP_IS_NOT_ALIVE(b);
    b->pos.x -= b->vx + scr;
    char* cd = "d";
    Collision cl = character(cd, VEC_XY(b->pos));
    if (cl.isColliding.character['a'] || cl.isColliding.character['b']) {
      play(EXPLOSION);
      if (chickCount > 0) {
        isFalling = true;
        bird.vy = 3 * sqrtf(difficulty);
      } else {
        gameOver();
      }
      b->isAlive = false;
    } else {
      b->isAlive = b->pos.x >= -3;
    }
  }
  color = BLACK;
  bool isHit = isFalling;
  isFalling = false;
  int nextChickCount = -1;
  for (int i = 0; i < chickCount; i++) {
    Chick* c = &(chicks[i]);
    c->targetIndex = 3 * (i + 1);
    c->index += (c->targetIndex - c->index) * 0.05;
    Vector p = bird.posHistory[(int)(c->index)];
    char* cc = "c";
    Collision cl = character(cc, VEC_XY(p));
    if (cl.isColliding.character['d']) {
      play(POWER_UP);
      isHit = true;
    }
    if (isHit) {
      ASSIGN_ARRAY_ITEM(fallingChicks, fallingChickIndex, FallingChick, fc);
      vectorSet(&fc->pos, VEC_XY(p));
      fc->vy = 0;
      fc->isAlive = true;
      fallingChickIndex =
          wrap(fallingChickIndex + 1, 0, IN_TOW_MAX_FALLING_CHICK_COUNT);
      if (nextChickCount < 0) {
        nextChickCount = i;
      }
    }
  }
  if (nextChickCount >= 0) {
    chickCount = nextChickCount;
  }
  FOR_EACH(fallingChicks, i) {
    ASSIGN_ARRAY_ITEM(fallingChicks, i, FallingChick, fc);
    SKIP_IS_NOT_ALIVE(fc);
    fc->vy += 0.3 * difficulty;
    fc->pos.y += fc->vy;
    characterOptions.isMirrorY = true;
    character("c", VEC_XY(fc->pos));
    characterOptions.isMirrorY = false;
    fc->isAlive = fc->pos.y <= 103;
  }
  color = BLACK;
  char ppc[2] = {bird.vy < 0 ? 'b' : 'a', '\0'};
  character(ppc, VEC_XY(bird.pos));
  if (bird.pos.y > 99) {
    play(EXPLOSION);
    gameOver();
  }
}

void addGameInTow() {
  addGame(title, description, characters, charactersCount, options, update);
}
