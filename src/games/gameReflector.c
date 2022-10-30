#include "cglp.h"

static char *title = "REFLECTOR";
static char *description = "[Tap]\n Turn\n[Hold]\n Enforce\n reflector";

#define CS static char characters[][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1]
CS = {{
          "      ",
          "y cc y",
          " cccc ",
          "llllll",
          "lllll ",
          " l l  ",
      },
      {
          "      ",
          "y cc y",
          " cccc ",
          "llllll",
          " lllll",
          "  l l ",
      },
      {
          "      ",
          " rrr  ",
          "rrrrr ",
          "rrrrrr",
          "lwlwll",
          " lwll ",
      },
      {
          "      ",
          " rrr  ",
          "rrrrr ",
          "rrrrrr",
          "llwlwl",
          " llwl ",
      }};
static int charactersCount = 4;

static Options options = {
    .viewSizeX = 100, .viewSizeY = 100, .soundSeed = 1, .isDarkColor = false};

typedef struct {
  Vector pos;
  float vx;
  float angle;
  float power;
} Ufo;
Ufo ufo;
typedef struct {
  Vector pos;
  float vx;
  float angle;
  float angleVel;
  float speed;
  float fireTicks;
  float fireInterval;
  float fireSpeed;
  bool isAlive;
} Tank;
#define REFLECTOR_MAX_TANK_COUNT 32
Tank tanks[REFLECTOR_MAX_TANK_COUNT];
int tankIndex;
float nextTankTicks;
typedef struct {
  Vector pos;
  Vector vel;
  bool isAlive;
} Bullet;
#define REFLECTOR_MAX_BULLET_COUNT 32
Bullet bullets[REFLECTOR_MAX_BULLET_COUNT];
int bulletIndex;
typedef struct {
  Vector pos;
  float radius;
  float ticks;
  float duration;
  bool isAlive;
} Explosion;
#define REFLECTOR_MAX_EXPLOSION_COUNT 8
Explosion explosions[REFLECTOR_MAX_EXPLOSION_COUNT];
int explosionIndex;
float multiplier;

static void update() {
  if (!ticks) {
    vectorSet(&ufo.pos, 40, 9);
    ufo.vx = 1;
    ufo.angle = 0;
    ufo.power = 0;
    INIT_UNALIVED_ARRAY(tanks);
    tankIndex = 0;
    INIT_UNALIVED_ARRAY(bullets);
    bulletIndex = 0;
    INIT_UNALIVED_ARRAY(explosions);
    explosionIndex = 0;
    multiplier = 1;
  }
  color = LIGHT_BLACK;
  rect(0, 90, 100, 10);
  if (input.isJustPressed || (ufo.pos.x < 3 && ufo.vx < 0) ||
      (ufo.pos.x > 97 && ufo.vx > 0)) {
    play(SELECT);
    ufo.vx *= -1;
  }
  if (input.isPressed) {
    ufo.power += (1 - ufo.power) * 0.05;
  } else {
    ufo.power *= 0.9;
  }
  ufo.pos.x += ufo.vx * sqrt(difficulty) * (input.isPressed ? 0.5 : 1);
  if (!input.isPressed) {
    ufo.angle = clamp(ufo.angle - ufo.vx * sqrt(difficulty) * 0.07, -M_PI / 4,
                      M_PI / 4);
  }
  color = BLACK;
  char uc[2] = {'a' + (int)(ticks / 15) % 2, '\0'};
  character(uc, VEC_XY(ufo.pos));
  color = BLUE;
  Vector up;
  addWithAngle(vectorSet(&up, VEC_XY(ufo.pos)), ufo.angle + M_PI / 2, 6);
  thickness = 3 + ufo.power * 3;
  bar(VEC_XY(up), 9 - ufo.power * 9, ufo.angle);
  thickness = 3;
  COUNT_IS_ALIVE(tanks, tc);
  if (tc == 0) {
    nextTankTicks = 0;
  }
  nextTankTicks--;
  if (nextTankTicks < 0) {
    ASSIGN_ARRAY_ITEM(tanks, tankIndex, Tank, t);
    float vx = RNDPM();
    float fireInterval = rnd(200, 300) / difficulty;
    float av = rnd(1, 5);
    vectorSet(&t->pos, vx > 0 ? -3 : 103, 87);
    t->vx = vx;
    t->angle = -M_PI / 2;
    t->angleVel = av * 0.002;
    t->speed = 0.1 / sqrt(av) * sqrt(difficulty);
    t->fireTicks = t->fireInterval = fireInterval;
    t->fireSpeed = rnd(1, 1.5) * sqrt(difficulty);
    t->isAlive = true;
    nextTankTicks = rnd(60, 80) / sqrt(difficulty);
    tankIndex = wrap(tankIndex + 1, 0, REFLECTOR_MAX_TANK_COUNT);
  }
  color = LIGHT_RED;
  COUNT_IS_ALIVE(explosions, ec);
  if (ec == 0) {
    multiplier = 1;
  }
  FOR_EACH(explosions, i) {
    ASSIGN_ARRAY_ITEM(explosions, i, Explosion, e);
    SKIP_IS_NOT_ALIVE(e);
    e->ticks--;
    float r = e->radius * sin(e->ticks / e->duration * M_PI);
    arc(VEC_XY(e->pos), r, 0, M_PI * 2);
    e->isAlive = e->ticks >= 0;
  }
  color = BLACK;
  thickness = 2;
  barCenterPosRatio = -0.5;
  FOR_EACH(tanks, i) {
    ASSIGN_ARRAY_ITEM(tanks, i, Tank, t);
    SKIP_IS_NOT_ALIVE(t);
    t->pos.x += t->vx * t->speed;
    float ta = angleTo(&t->pos, VEC_XY(ufo.pos));
    if (fabsf(ta) < t->angleVel) {
      t->angle = ta;
    } else if (ta < t->angle) {
      t->angle -= t->angleVel;
    } else {
      t->angle += t->angleVel;
    }
    bar(VEC_XY(t->pos), 3, t->angle);
    char tc[2] = {'c' + (int)(ticks / 25) % 2, '\n'};
    characterOptions.isMirrorX = t->vx < 0;
    if (character(tc, VEC_XY(t->pos)).isColliding.rect[LIGHT_RED]) {
      play(POWER_UP);
      addScore(multiplier, VEC_XY(t->pos));
      multiplier++;
      particle(VEC_XY(t->pos), 16, 1, 0, M_PI * 2);
      t->isAlive = false;
      continue;
    }
    t->fireTicks--;
    if (t->fireTicks < 0) {
      play(LASER);
      ASSIGN_ARRAY_ITEM(bullets, bulletIndex, Bullet, b);
      vectorSet(&b->pos, VEC_XY(t->pos));
      rotate(vectorSet(&b->vel, t->fireSpeed, 0), t->angle);
      b->isAlive = true;
      bulletIndex = wrap(bulletIndex + 1, 0, REFLECTOR_MAX_BULLET_COUNT);
      t->fireTicks = t->fireInterval;
    }
    t->isAlive = !(t->pos.x < -3 || t->pos.x > 103);
  }
  thickness = 3;
  barCenterPosRatio = 0.5;
  FOR_EACH(bullets, i) {
    ASSIGN_ARRAY_ITEM(bullets, i, Bullet, b);
    SKIP_IS_NOT_ALIVE(b);
    vectorAdd(&b->pos, VEC_XY(b->vel));
    Collisions c = bar(VEC_XY(b->pos), 4, vectorAngle(&b->vel)).isColliding;
    if (b->vel.y < 0) {
      if (c.rect[BLUE]) {
        play(COIN);
        particle(VEC_XY(b->pos), 9, 1 + ufo.power * 2, ufo.angle + M_PI / 2,
                 M_PI / 8);
        float ra = vectorAngle(&b->vel) - ufo.angle;
        float a = vectorAngle(&b->vel);
        float s = vectorLength(&b->vel) * (1 + ufo.power * 4);
        addWithAngle(vectorSet(&b->vel, 0, 0), a - ra * 2, s);
        addWithAngle(&b->pos, a - ra * 2, s * 2);
        if (b->pos.y < 20) {
          b->pos.y = 20;
        }
      } else if (c.character['a'] || c.character['b']) {
        play(RANDOM);
        gameOver();
      }
    }
    if (b->vel.y > 0 &&
        (c.character['c'] || c.character['d'] || c.rect[LIGHT_BLACK])) {
      play(EXPLOSION);
      float s = vectorLength(&b->vel) / sqrt(difficulty);
      float radius = s * s;
      float duration = sqrt(radius) * 9;
      ASSIGN_ARRAY_ITEM(explosions, explosionIndex, Explosion, e);
      vectorSet(&e->pos, VEC_XY(b->pos));
      e->radius = radius;
      e->ticks = e->duration = duration;
      e->isAlive = true;
      explosionIndex =
          wrap(explosionIndex + 1, 0, REFLECTOR_MAX_EXPLOSION_COUNT);
      b->isAlive = false;
      continue;
    }
    b->isAlive =
        b->pos.x > -3 && b->pos.x < 106 && b->pos.y > -3 && b->pos.y < 106;
  }
}

void addGameReflector() {
  addGame(title, description, characters, charactersCount, options, update);
}
