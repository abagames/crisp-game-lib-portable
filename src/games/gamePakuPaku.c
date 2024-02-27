#include "cglp.h"

static char *title = "PAKU PAKU";
static char *description = "[Tap]   Turn\n[Arrow] Move";

#define CS static char characters[][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1]
CS = {
    {
        "  llll",
        " lll  ",
        "lll   ",
        "lll   ",
        " lll  ",
        "  llll",
    },
    {
        "  lll ",
        " lllll",
        "lll   ",
        "lll   ",
        " lllll",
        "  lll ",
    },
    {
        "  ll  ",
        " llll ",
        "llllll",
        "llllll",
        " llll ",
        "  ll  ",
    },
    {
        "  lll ",
        " l l l",
        " llll ",
        " llll ",
        "llll  ",
        "l l l ",
    },
    {
        "  lll ",
        " l l l",
        " llll ",
        " llll ",
        " llll ",
        " l l  ",
    },
    {
        "      ",
        "      ",
        "  ll  ",
        "  ll  ",
        "      ",
        "      ",
    },
    {
        "      ",
        "  ll  ",
        " llll ",
        " llll ",
        "  ll  ",
        "      ",
    },
    {
        "      ",
        "   l l",
        "      ",
        "      ",
        "      ",
        "      ",
    },
    {
        "      ",
        "  lll ",
        " l   l",
        " llll ",
        "llll  ",
        "l l   ",
    },
    {
        "       ",
        "  lll ",
        " l   l",
        " llll ",
        " llll ",
        " l l  ",
    },
};
static int charactersCount = 10;

static Options options = {
    .viewSizeX = 100, .viewSizeY = 50, .soundSeed = 4, .isDarkColor = true};

typedef struct {
  float x;
  float vx;
} Player;
Player player;
typedef struct {
  float x;
  float eyeVx;
} Enemy;
Enemy enemy;
typedef struct {
  float x;
  bool isPower;
  bool isAlive;
} Dot;
#define PAKU_PAKU_MAX_DOT_COUNT 16
Dot dots[PAKU_PAKU_MAX_DOT_COUNT];
float powerTicks;
float animTicks;
int multiplier;

static void addDots() {
  int pi = player.x > 50 ? rndi(1, 6) : rndi(10, 15);
  for (int i = 0; i < 16; i++) {
    dots[i].x = i * 6 + 5;
    dots[i].isPower = i == pi;
    dots[i].isAlive = true;
  }
  multiplier++;
}

static void update() {
  if (!ticks) {
    player.x = 40;
    player.vx = 1;
    enemy.x = 100;
    enemy.eyeVx = 0;
    multiplier = 1;
    addDots();
    powerTicks = animTicks = 0;
  }
  animTicks += difficulty;
  color = BLACK;
  text("x", 3, 9);
  text(intToChar(multiplier), 9, 9);
  bool isJustPressed = input.isJustPressed;
  if (isJustPressed && ((input.left.isJustPressed && player.vx < 0) ||
                        (input.right.isJustPressed && player.vx > 0))) {
    isJustPressed = false;
  }
  if (isJustPressed) {
    player.vx *= -1;
  }
  player.x += player.vx * 0.5 * difficulty;
  if (player.x < -3) {
    player.x = 103;
  } else if (player.x > 103) {
    player.x = -3;
  }
  color = BLUE;
  rect(0, 23, 100, 1);
  rect(0, 25, 100, 1);
  rect(0, 34, 100, 1);
  rect(0, 36, 100, 1);
  color = GREEN;
  int ai = ((int)animTicks / 7) % 4;
  characterOptions.isMirrorX = player.vx < 0;
  char pc[2] = {'a' + (ai == 3 ? 1 : ai), '\0'};
  character(pc, player.x, 30);
  characterOptions.isMirrorX = false;
  FOR_EACH(dots, i) {
    ASSIGN_ARRAY_ITEM(dots, i, Dot, d);
    SKIP_IS_NOT_ALIVE(d);
    color = d->isPower && ((int)animTicks / 7) % 2 == 0 ? TRANSPARENT : YELLOW;
    char dc[2] = {d->isPower ? 'g' : 'f', '\0'};
    Collision c = character(dc, d->x, 30);
    if (c.isColliding.character['a'] || c.isColliding.character['b'] ||
        c.isColliding.character['c']) {
      if (d->isPower) {
        play(JUMP);
        if (enemy.eyeVx == 0) {
          powerTicks = 120;
        }
      } else {
        play(HIT);
      }
      score += multiplier;
      d->isAlive = false;
    }
  }
  float evx = enemy.eyeVx != 0
                  ? enemy.eyeVx
                  : (player.x > enemy.x ? 1 : -1) * (powerTicks > 0 ? -1 : 1);
  enemy.x = clamp(
      enemy.x + evx *
                    (powerTicks > 0 ? 0.25 : (enemy.eyeVx != 0 ? 0.75 : 0.55)) *
                    difficulty,
      0, 100);
  if ((enemy.eyeVx < 0 && enemy.x < 1) || (enemy.eyeVx > 0 & enemy.x > 99)) {
    enemy.eyeVx = 0;
  }
  if (enemy.eyeVx != 0) {
    color = BLACK;
  } else if (powerTicks <= 0) {
    color = RED;
  } else {
    color = BLUE;
    if (powerTicks < 30 && (int)powerTicks % 10 < 5) {
      color = TRANSPARENT;
    }
  }
  characterOptions.isMirrorX = evx < 0;
  char ec[2] = {enemy.eyeVx != 0
                    ? 'h'
                    : (powerTicks > 0 ? 'i' : 'd') + ((int)animTicks / 7) % 2,
                '\0'};
  Collision c = character(ec, enemy.x, 30);
  characterOptions.isMirrorX = false;
  if (enemy.eyeVx == 0 &&
      (c.isColliding.character['a'] || c.isColliding.character['b'] ||
       c.isColliding.character['c'])) {
    if (powerTicks > 0) {
      play(POWER_UP);
      addScore(10 * multiplier, enemy.x, 30);
      enemy.eyeVx = player.x > 50 ? -1 : 1;
      powerTicks = 0;
      multiplier++;
    } else {
      play(EXPLOSION);
      gameOver();
    }
  }
  powerTicks -= difficulty;
  COUNT_IS_ALIVE(dots, dc);
  if (dc == 0) {
    play(COIN);
    addDots();
  }
}

void addGamePakuPaku() {
  addGame(title, description, characters, charactersCount, options, update);
}
