#include "cglp.h"

static char *title = "LADDER DROP";
static char *description = "[Tap] Drop";

#define CS static char characters[][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1]
CS = {{
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
      },
      {
          "llllll",
          "llllll",
          "llllll",
          "llllll",
          " l  l ",
          " l  l ",
      },
      {
          "      ",
          "llllll",
          "llllll",
          "llllll",
          "llllll",
          "ll  ll",
      },
      {
          "b    b",
          "bbbbbb",
          "b    b",
          "b    b",
          "bbbbbb",
          "b    b",
      },
      {
          "LLLLLL",
          "r rr r",
          "r rr r",
          "      ",
          "rr rr ",
          "rr rr ",
      },
      {
          "b    b",
          "bbbbbb",
          "b    b",
          "b    b",
          "bbbbbb",
          "b    b",
      },
      {
          "RRRRRR",
          "rrrrrr",
          "rrrrrr",
          "rrrrrr",
          "rrrrrr",
          "rrrrrr",
      },
      {
          "      ",
          "  yyy ",
          " yYYYy",
          " yYyYy",
          " yYYYy",
          "  yyy ",
      }};
static int charactersCount = 9;

static Options options = {
    .viewSizeX = 100, .viewSizeY = 100, .soundSeed = 2, .isDarkColor = false};

typedef struct {
  Vector pos;
  Vector size;
  int lxs[3];
  int lxsCount;
  int state;
  bool hasCoin;
  bool isAlive;
} Panel;
#define LADDER_DROP_MAX_PANEL_COUNT 32
static Panel panels[LADDER_DROP_MAX_PANEL_COUNT];
static int panelIndex;
static float pvx;
static float nextPanelX;
static int coinPanelCount;
typedef struct {
  Vector pos;
  int vx;
  int state;
} Player;
static Player player;
static int lockCount;
typedef struct {
  Vector pos;
  bool isAlive;
} Coin;
#define LADDER_DROP_MAX_COIN_COUNT 32
static Coin coins[LADDER_DROP_MAX_COIN_COUNT];
static int coinIndex;
static float scr;
static float totalScr;
static int multiplier;
static int coinPanelInterval = 4;

static void addPanel() {
  ASSIGN_ARRAY_ITEM(panels, panelIndex, Panel, p);
  panelIndex = wrap(panelIndex + 1, 0, LADDER_DROP_MAX_PANEL_COUNT);
  Vector size;
  vectorSet(&size, rndi(4, 8), rndi(3, 6));
  int lx = -1;
  int nd = rndi(1, 5);
  p->lxsCount = 0;
  while (lx < size.x) {
    lx += nd;
    p->lxs[p->lxsCount] = lx;
    p->lxsCount++;
    lx += rndi(2, 5);
  }
  bool hasCoin = false;
  coinPanelCount--;
  if (coinPanelCount == 0) {
    coinPanelCount = coinPanelInterval;
    hasCoin = true;
  }
  vectorSet(&p->pos, clamp(nextPanelX, 2, 98 - size.x * 6), 0);
  vectorSet(&p->size, VEC_XY(size));
  p->state = 0;
  p->hasCoin = hasCoin;
  p->isAlive = true;
}

static bool cl[10];

static bool *drawPanel(Panel *p) {
  char lc = p->state == 2 ? 'e' : 'g';
  char fc = p->state == 1 ? 'h' : 'f';
  unsigned char c;
  int li = 0;
  for (int i = 0; i < 10; i++) {
    cl[i] = false;
  }
  char cc[2] = {'\0', '\0'};
  TIMES(p->size.x, x) {
    TIMES(p->size.y, y) {
      c = 0;
      if (y == 0 && p->hasCoin) {
        c = 'i';
      } else if (y >= 1 && li < p->lxsCount && x == p->lxs[li]) {
        c = lc;
      } else if (y == 1) {
        c = fc;
      }
      if (c > 0) {
        cc[0] = c;
        bool *clc = character(cc, p->pos.x + x * 6 + 3, p->pos.y + y * 6 + 3)
                        .isColliding.character;
        for (int i = 0; i < 10; i++) {
          cl[i] |= clc[i + 'a'];
        }
      }
    }
    if (x == p->lxs[li]) {
      li++;
    }
  }
  return cl;
}

static void update() {
  if (!ticks) {
    INIT_UNALIVED_ARRAY(panels);
    ASSIGN_ARRAY_ITEM(panels, 0, Panel, p);
    vectorSet(&p->pos, 2, 88);
    vectorSet(&p->size, 16, 2);
    p->lxsCount = 0;
    p->state = 2;
    p->hasCoin = false;
    p->isAlive = true;
    panelIndex = 1;
    pvx = 1;
    nextPanelX = 50;
    coinPanelCount = coinPanelInterval;
    addPanel();
    vectorSet(&player.pos, 9, 91);
    player.vx = 1;
    player.state = 0;
    INIT_UNALIVED_ARRAY(coins);
    coinIndex = 0;
    scr = totalScr = 0;
    multiplier = 1;
    lockCount = 0;
  }
  color = LIGHT_BLUE;
  rect(0, 0, 2, 100);
  rect(98, 0, 2, 100);
  color = BLACK;
  float minY = 99;
  FOR_EACH(panels, i) {
    ASSIGN_ARRAY_ITEM(panels, i, Panel, p);
    SKIP_IS_NOT_ALIVE(p);
    if (p->state == 0) {
      p->pos.x += pvx * sqrt(difficulty) * 1.5;
      nextPanelX = p->pos.x;
      if (p->pos.x < -9) {
        pvx *= -1;
        p->pos.x = -9;
      } else if (p->pos.x > 109 - p->size.x * 6) {
        pvx *= -1;
        p->pos.x = 109 - p->size.x * 6;
      }
      drawPanel(p);
      if (input.isJustPressed) {
        play(SELECT);
        p->state = 1;
      }
    } else if (p->state == 1) {
      p->pos.y += 6 * sqrt(difficulty);
      color = TRANSPARENT;
      bool *cl = drawPanel(p);
      if (cl['e' - 'a'] || cl['f' - 'a']) {
        while (cl['e' - 'a'] || cl['f' - 'a']) {
          p->pos.y--;
          cl = drawPanel(p);
        }
        p->pos.y = floorf(p->pos.y) + fmodf(totalScr, 1);
        p->state = 2;
        if (p->hasCoin) {
          p->hasCoin = false;
          TIMES(p->size.x, x) {
            ASSIGN_ARRAY_ITEM(coins, coinIndex, Coin, c);
            vectorSet(&c->pos, p->pos.x + x * 6 + 2, p->pos.y + 2);
            c->isAlive = true;
            coinIndex = wrap(coinIndex + 1, 0, LADDER_DROP_MAX_COIN_COUNT);
          }
        }
        addPanel();
      }
      color = BLACK;
      drawPanel(p);
    } else if (p->state == 2) {
      p->pos.y += scr;
      color = BLACK;
      drawPanel(p);
      if (p->pos.y < minY) {
        minY = p->pos.y;
      }
    }
    if (p->pos.y > 99) {
      if (p->state == 1) {
        addPanel();
      }
      p->isAlive = false;
    }
  }
  color = BLACK;
  player.pos.y += scr;
  //  player.state: "walk" | "up" | "down" | "downWalk" | "drop"
  if (player.state == 0 || player.state == 3) {
    player.pos.x += player.vx * sqrt(difficulty) * 0.4;
    char pc[2] = {'a' + (int)(ticks / 30) % 2, '\0'};
    characterOptions.isMirrorX = player.vx < 0;
    bool *c = character(pc, VEC_XY(player.pos)).isColliding.character;
    characterOptions.isMirrorX = false;
    if (c['h']) {
      play(EXPLOSION);
      gameOver();
    }
    if (c['f'] || player.pos.x < 5 || player.pos.x > 95) {
      play(LASER);
      player.vx *= -1;
      player.pos.x += player.vx * 2;
      lockCount++;
      if (lockCount > 8) {
        player.pos.x = clamp(player.pos.x, 10, 90);
        player.pos.y -= 6;
        lockCount = 0;
        player.state = 4;
      }
    } else {
      lockCount = 0;
    }
    if (c['e']) {
      if (player.state == 0) {
        player.state = 1;
      }
    } else {
      player.state = 0;
      color = TRANSPARENT;
      bool *c =
          character("a", player.pos.x, player.pos.y + 6).isColliding.character;
      if (!(c['e'] || c['f'])) {
        player.state = 4;
      }
    }
  } else if (player.state == 1) {
    play(HIT);
    player.pos.y -= sqrt(difficulty) * 0.3;
    color = TRANSPARENT;
    bool *c = character("c", VEC_XY(player.pos)).isColliding.character;
    if (!c['e'] && c['f']) {
      player.state = 2;
    } else if (!c['e']) {
      float py = player.pos.y;
      while (c['e']) {
        py++;
        c = character("c", player.pos.x, py).isColliding.character;
      }
      player.pos.y = floorf(py) + fmodf(totalScr, 1);
      player.pos.x += player.vx * sqrt(difficulty) * 0.5;
      player.state = 0;
    }
    color = BLACK;
    char pc[2] = {'c' + (int)(ticks / 30) % 2, '\0'};
    character(pc, VEC_XY(player.pos));
  } else if (player.state == 2) {
    play(HIT);
    player.pos.y += sqrt(difficulty) * 0.4;
    color = TRANSPARENT;
    bool *c =
        character("c", player.pos.x, player.pos.y + 6).isColliding.character;
    if (!c['e'] && c['f']) {
      float py = player.pos.y + 6;
      while (c['f']) {
        py--;
        c = character("c", player.pos.x, py).isColliding.character;
      }
      player.pos.y = floorf(py) + fmodf(totalScr, 1);
      player.state = 3;
    }
    color = BLACK;
    char pc[2] = {'c' + (int)(ticks / 30) % 2, '\0'};
    character(pc, VEC_XY(player.pos));
  } else {
    player.pos.y += sqrt(difficulty) * 0.5;
    color = TRANSPARENT;
    bool *c = character("a", VEC_XY(player.pos)).isColliding.character;
    if (c['e'] || c['f']) {
      float py = player.pos.y;
      while (!(c['e'] || c['f'])) {
        py--;
        c = character("a", VEC_XY(player.pos)).isColliding.character;
      }
      player.pos.y = floorf(py - 1) + fmodf(totalScr, 1);
      player.state = 0;
    }
    color = BLACK;
    characterOptions.isMirrorX = player.vx < 0;
    character("a", VEC_XY(player.pos));
    characterOptions.isMirrorX = false;
  }
  if (player.pos.y > 99) {
    play(EXPLOSION);
    gameOver();
  }
  color = BLACK;
  FOR_EACH(coins, i) {
    ASSIGN_ARRAY_ITEM(coins, i, Coin, c);
    SKIP_IS_NOT_ALIVE(c);
    c->pos.y += scr;
    bool *cl = character("i", VEC_XY(c->pos)).isColliding.character;
    if (cl['a'] || cl['b']) {
      play(COIN);
      addScore(multiplier, VEC_XY(c->pos));
      multiplier++;
      c->isAlive = false;
    }
    if (c->pos.y > 36 && (cl['e'] || cl['f'])) {
      c->isAlive = false;
    }
    if (c->pos.y > 103) {
      if (multiplier > 1) {
        multiplier--;
      }
      c->isAlive = false;
    }
  }
  scr = 0.01 * difficulty;
  if (minY < 30) {
    scr += (30 - minY) * 0.1;
  }
  totalScr += scr;
}

void addGameLadderDrop() {
  addGame(title, description, characters, charactersCount, options, update);
}
