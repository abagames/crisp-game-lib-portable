#include "cglp.h"

static char *title = "R WHEEL";
static char *description = "[Tap]\n Multiple jumps";

#define CS static char characters[][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1]
CS = {{
          "llllll",
          "ll l l",
          "ll l l",
          "llllll",
          " l  l",
          " l  l",
      },
      {
          "      ",
          "llllll",
          "ll l l",
          "ll l l",
          "llllll",
          "ll  ll",
      },
      {
          " llll ",
          "  ll  ",
          "      ",
          " llll ",
          "      ",
          "llllll",
      }};
static int charactersCount = 3;

static Options options = {
    .viewSizeX = 100, .viewSizeY = 100, .soundSeed = 4, .isDarkColor = false};

typedef struct {
  int height;
  bool isHit;
} Spike;
#define RWHEEL_SPIKE_COUNT 32
static Spike spikes[RWHEEL_SPIKE_COUNT];
float angleOfs;
typedef struct {
  float angle;
  float radius;
  bool isAlive;
} Bonus;
#define RWHEEL_MAX_BONUS_COUNT 8
static Bonus bonuses[RWHEEL_MAX_BONUS_COUNT];
static int bonusIndex;
typedef struct {
  float y;
  float vy;
} Player;
static Player player;
typedef struct {
  Vector pos;
  float width;
  bool isSpike;
  bool isAlive;
} Bar;
#define RWHEEL_MAX_BAR_COUNT 8
static Bar bars[RWHEEL_MAX_BAR_COUNT];
static int barIndex;
static int multiplier;
static int validSpikeCount;
static float wheelRadius = 40;

static void addBonus() {
  TIMES((int)ceilf((float)validSpikeCount / 9), i) {
    ASSIGN_ARRAY_ITEM(bonuses, bonusIndex, Bonus, b);
    b->angle = -M_PI / 3 * 2 + rnd(0, M_PI / 4 * 3) * RNDPM();
    b->radius = rnd(10, 30);
    b->isAlive = true;
    bonusIndex = wrap(bonusIndex + 1, 0, RWHEEL_MAX_BONUS_COUNT);
  }
}

static void update() {
  if (!ticks) {
    FOR_EACH(spikes, i) {
      ASSIGN_ARRAY_ITEM(spikes, i, Spike, s);
      s->height = 0;
      s->isHit = false;
    }
    angleOfs = 0;
    INIT_UNALIVED_ARRAY(bonuses);
    bonusIndex = 0;
    player.y = 0;
    player.vy = 0;
    INIT_UNALIVED_ARRAY(bars);
    barIndex = 0;
    multiplier = 1;
    validSpikeCount = 0;
  }
  float sd = sqrt(difficulty);
  if (input.isJustPressed) {
    play(JUMP);
    player.vy = -2 * sd;
    FOR_EACH(spikes, i) {
      ASSIGN_ARRAY_ITEM(spikes, i, Spike, s);
      s->isHit = false;
    }
    if (player.y == 0) {
      player.y += player.vy;
      multiplier = 0;
      addBonus();
    }
  }
  if (player.y < 0) {
    float pvy = player.vy;
    player.vy += (input.isPressed ? 1 : 3) * 0.03 * difficulty;
    player.vy *= 0.98;
    if (player.y < -wheelRadius * 2 + 6 && player.vy < 0) {
      player.vy *= -0.5;
    }
    if (pvy * player.vy <= 0) {
      play(LASER);
      ASSIGN_ARRAY_ITEM(bars, barIndex, Bar, b);
      vectorSet(&b->pos, 50, 50 + wheelRadius + player.y);
      b->width = 0;
      b->isSpike = true;
      b->isAlive = true;
      barIndex = wrap(barIndex + 1, 0, RWHEEL_MAX_BAR_COUNT);
    }
    player.y += player.vy;
    if (player.y > 0) {
      player.y = player.vy = 0;
    }
  }
  color = BLACK;
  char pc[2] = {'a' + (player.y > 0 ? 0 : (int)(ticks / 10) % 2), '\0'};
  character(pc, 50, 50 + wheelRadius + player.y - 3);
  float va = 0.03 * sd;
  color = YELLOW;
  FOR_EACH(bonuses, i) {
    ASSIGN_ARRAY_ITEM(bonuses, i, Bonus, b);
    SKIP_IS_NOT_ALIVE(b);
    Vector p;
    addWithAngle(vectorSet(&p, 50, 50), b->angle, b->radius);
    b->angle += va;
    bool *c = character("c", VEC_XY(p)).isColliding.character;
    if (c['a'] || c['b']) {
      play(COIN);
      ASSIGN_ARRAY_ITEM(bars, barIndex, Bar, ba);
      vectorSet(&ba->pos, 50, 50 + wheelRadius + player.y);
      ba->width = 0;
      ba->isSpike = false;
      ba->isAlive = true;
      barIndex = wrap(barIndex + 1, 0, RWHEEL_MAX_BAR_COUNT);
      b->isAlive = false;
    }
  }
  FOR_EACH(bars, i) {
    ASSIGN_ARRAY_ITEM(bars, i, Bar, b);
    SKIP_IS_NOT_ALIVE(b);
    if (b->isSpike) {
      b->width += sd;
      b->pos.y += sd * 3;
      color = PURPLE;
    } else {
      b->width += sd * 2;
      b->pos.y += sd * 2;
      color = YELLOW;
    }
    box(VEC_XY(b->pos), b->width, 3);
    b->isAlive = b->pos.y < 103;
  }
  angleOfs += va;
  color = BLACK;
  arc(50, 50, wheelRadius + 3, angleOfs, angleOfs + M_PI * 2);
  float a = angleOfs;
  validSpikeCount = 0;
  FOR_EACH(spikes, i) {
    ASSIGN_ARRAY_ITEM(spikes, i, Spike, s);
    color = s->height > 0 ? RED : TRANSPARENT;
    Vector p;
    addWithAngle(vectorSet(&p, 50, 50), a, wheelRadius * (1 - s->height * 0.1));
    Vector bp;
    addWithAngle(vectorSet(&bp, 50, 50), a, wheelRadius);
    float l = 0.05 + s->height * 0.1;
    Vector bp2;
    addWithAngle(vectorSet(&bp2, VEC_XY(bp)), a - M_PI / 2,
                 50 / RWHEEL_SPIKE_COUNT);
    Collisions c1 = line(VEC_XY(p), VEC_XY(bp2)).isColliding;
    addWithAngle(vectorSet(&bp2, VEC_XY(bp)), a + M_PI / 2,
                 50 / RWHEEL_SPIKE_COUNT);
    Collisions c2 = line(VEC_XY(p), VEC_XY(bp2)).isColliding;
    if (!s->isHit && (c1.rect[PURPLE] || c2.rect[PURPLE])) {
      play(HIT);
      s->height++;
      s->isHit = true;
    }
    if (s->height > 0) {
      if (c1.rect[YELLOW] || c2.rect[YELLOW]) {
        play(SELECT);
        multiplier += s->height;
        addScore(multiplier, VEC_XY(p));
        s->height = 0;
      } else if (c1.character['a'] || c2.character['a'] || c1.character['b'] ||
                 c2.character['b']) {
        play(EXPLOSION);
        gameOver();
      }
    }
    a += M_PI * 2 / RWHEEL_SPIKE_COUNT;
    if (s->height > 0) {
      validSpikeCount++;
    }
  }
  if (validSpikeCount == 0) {
    spikes[(int)wrap(
               (int)(-angleOfs - M_PI / 4) / (M_PI * 2 / RWHEEL_SPIKE_COUNT), 0,
               RWHEEL_SPIKE_COUNT)]
        .height = 1;
    spikes[(int)wrap((int)(-angleOfs - M_PI / 4 * 3) /
                         (M_PI * 2 / RWHEEL_SPIKE_COUNT),
                     0, RWHEEL_SPIKE_COUNT)]
        .height = 1;
    addBonus();
  }
}

void addGameRWheel() {
  addGame(title, description, characters, charactersCount, options, update);
}
