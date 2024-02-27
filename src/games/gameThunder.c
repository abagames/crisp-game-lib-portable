#include "cglp.h"

static char *title = "THUNDER";
static char *description = "[Tap]   Turn\n[Arrow] Move";

#define CS static char characters[][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1]
CS = {{
          "      ",
          "      ",
          "   l  ",
          "  lll ",
          "  l l ",
          "      ",
      },
      {
          "llllll",
          "ll l l",
          "ll l l",
          "llllll",
          " l  l ",
          " l  l ",
      },
      {
          "      ",
          "llllll",
          "ll l l",
          "ll l l",
          "llllll",
          "ll  ll",
      }};
static int charactersCount = 3;

static Options options = {
    .viewSizeX = 100, .viewSizeY = 100, .soundSeed = 9, .isDarkColor = true};

typedef struct _Line {
  Vector from;
  Vector to;
  Vector vel;
  float ticks;
  struct _Line *prevLine;
  bool isActive;
  bool isAlive;
} Line;
#define THUNDER_MAX_LINE_COUNT 128
static Line lines[THUNDER_MAX_LINE_COUNT];
static int lineIndex;
static int activeTicks;
typedef struct {
  Vector pos;
  float vy;
  bool isAlive;
} Star;
#define THUNDER_MAX_STAR_COUNT 32
static Star stars[THUNDER_MAX_STAR_COUNT];
static int starIndex;
typedef struct {
  float x;
  float vx;
} Player;
static Player player;
static int multiplier;

static void update() {
  if (!ticks) {
    INIT_UNALIVED_ARRAY(lines);
    lineIndex = 0;
    INIT_UNALIVED_ARRAY(stars);
    starIndex = 0;
    activeTicks = -1;
    player.x = 40;
    player.vx = 1;
    multiplier = 1;
  }
  COUNT_IS_ALIVE(lines, lc);
  if (lc == 0) {
    ASSIGN_ARRAY_ITEM(lines, lineIndex, Line, l);
    vectorSet(&l->from, rnd(30, 70), 0);
    vectorSet(&l->to, VEC_XY(l->from));
    vectorSet(&l->vel, 0.5 * difficulty, 0);
    rotate(&l->vel, M_PI / 2);
    l->ticks = ceilf(30.0f / difficulty);
    l->prevLine = NULL;
    l->isActive = false;
    l->isAlive = true;
    lineIndex = wrap(lineIndex + 1, 0, THUNDER_MAX_LINE_COUNT);
  }
  color = LIGHT_BLUE;
  rect(0, 90, 100, 10);
  activeTicks--;
  FOR_EACH(lines, i) {
    ASSIGN_ARRAY_ITEM(lines, i, Line, l);
    SKIP_IS_NOT_ALIVE(l);
    if (l->isActive) {
      color = YELLOW;
      line(VEC_XY(l->from), VEC_XY(l->to));
      l->isAlive = activeTicks >= 0;
      continue;
    }
    l->ticks--;
    if (activeTicks > 0) {
      if (l->ticks > 0) {
        ASSIGN_ARRAY_ITEM(stars, starIndex, Star, s);
        vectorSet(&s->pos, VEC_XY(l->to));
        s->vy = -l->to.y * 0.02;
        s->isAlive = true;
        starIndex = wrap(starIndex + 1, 0, THUNDER_MAX_STAR_COUNT);
      }
      l->isAlive = false;
      continue;
    }
    if (l->ticks > 0) {
      vectorAdd(&l->to, VEC_XY(l->vel));
      if (activeTicks < 0 && (l->to.y > 90 || lc >= THUNDER_MAX_LINE_COUNT)) {
        play(EXPLOSION);
        Line *al = l;
        color = YELLOW;
        for (int j = 0; j < 99; j++) {
          particle(VEC_XY(al->to), 30, 2, 0, M_PI * 2);
          al->isActive = true;
          al = al->prevLine;
          if (al == NULL) {
            break;
          }
        }
        activeTicks = ceilf(20.0f / sqrtf(difficulty));
        multiplier = 1;
      }
    } else if (l->ticks == 0) {
      play(HIT);
      color = BLACK;
      particle(VEC_XY(l->to), 9, 1, 0, M_PI * 2);
      for (int j = 0; j < rndi(1, 4); j++) {
        ASSIGN_ARRAY_ITEM(lines, lineIndex, Line, nl);
        vectorSet(&nl->from, VEC_XY(l->to));
        vectorSet(&nl->to, VEC_XY(l->to));
        vectorSet(&nl->vel, VEC_XY(l->vel));
        vectorMul(&nl->vel, 1.0 / vectorLength(&nl->vel));
        rotate(&nl->vel, rnd(-0.7, 0.7));
        vectorMul(&nl->vel, rnd(0.3, 1) * sqrt(difficulty));
        nl->ticks = ceilf(rnd(20, 40) / difficulty);
        nl->prevLine = l;
        nl->isActive = false;
        nl->isAlive = true;
        lineIndex = wrap(lineIndex + 1, 0, THUNDER_MAX_LINE_COUNT);
      }
    }
    color = LIGHT_BLACK;
    hasCollision = false;
    thickness = 2;
    line(VEC_XY(l->from), VEC_XY(l->to));
    thickness = 3;
    hasCollision = true;
  }
  bool isJustPressed = input.isJustPressed;
  if (isJustPressed && ((input.left.isJustPressed && player.vx < 0) ||
                        (input.right.isJustPressed && player.vx > 0))) {
    isJustPressed = false;
  }
  if (isJustPressed || (player.x < 0 && player.vx < 0) ||
      (player.x > 99 && player.vx > 0)) {
    play(LASER);
    player.vx *= -1;
  }
  player.x += player.vx * sqrtf(difficulty);
  color = BLACK;
  characterOptions.isMirrorX = player.vx < 0;
  char pc[2] = {'b' + (ticks / 10) % 2, '\0'};
  if (character(pc, player.x, 87).isColliding.rect[YELLOW]) {
    play(RANDOM);
    gameOver();
  }
  characterOptions.isMirrorX = false;
  color = YELLOW;
  FOR_EACH(stars, i) {
    ASSIGN_ARRAY_ITEM(stars, i, Star, s);
    SKIP_IS_NOT_ALIVE(s);
    s->vy += 0.1 * difficulty;
    s->pos.y += s->vy;
    if (s->pos.y > 89 && s->vy > 0) {
      s->vy *= -0.5;
      if (s->vy > -0.1) {
        s->isAlive = false;
        continue;
      }
    }
    bool *c = character("a", VEC_XY(s->pos)).isColliding.character;
    if (c['b'] || c['c']) {
      play(COIN);
      addScore(multiplier, VEC_XY(s->pos));
      multiplier++;
      s->isAlive = false;
    }
  }
}

void addGameThunder() {
  addGame(title, description, characters, charactersCount, options, update);
}
