#include "cglp.h"

static char *title = "TIMBER TEST";
static char *description = "[Tap] Cut a log";

#define CS static char characters[][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1]
CS = {
    {
        "      ",
        "      ",
        "      ",
        "      ",
        "      ",
        "      ",
    },
};
static int charactersCount = 1;

static Options options = {
    .viewSizeX = 100, .viewSizeY = 100, .soundSeed = 18, .isDarkColor = false};

typedef struct {
  float x;
  float width;
} Timber;
static Timber timber;
static int cutCount;
static int cutIndex;
typedef struct {
  Vector size;
  Vector pos;
  Vector targetPos;
  bool isAlive;
} Piece;
#define TIMBER_TEST_MAX_PIECE_COUNT 64
static Piece pieces[TIMBER_TEST_MAX_PIECE_COUNT];
static int pieceIndex;
typedef struct {
  float x;
  float vx;
} Saw;
static Saw saw;
static int scoreCountTicks;
static int scoreCountIndex;
static int turnScore;
static int turnIndex;
static bool isShowingIndicator;

static void nextTimber();

static void update() {
  if (!ticks) {
    turnIndex = 0;
    pieceIndex = 0;
    nextTimber();
  }
  color = RED;
  rect(timber.x, 20, timber.width, 10);
  saw.x += saw.vx;
  color = BLACK;
  rect(saw.x - 1, 10, 3, input.isJustPressed ? 30 : 7);
  char cutText[4] = {'1', '/', '1', '\0'};
  cutText[2] = '0' + cutCount;
  text(cutText, 5, 35);
  if (scoreCountTicks == 0 && saw.x >= timber.x + timber.width) {
    ASSIGN_ARRAY_ITEM(pieces, pieceIndex, Piece, p);
    vectorSet(&p->size, timber.width, 10);
    vectorSet(&p->pos, timber.x + p->size.x / 2, 20 + 10 / 2);
    vectorSet(&p->targetPos, 50, 40 + cutIndex * 15);
    p->isAlive = true;
    pieceIndex = wrap(pieceIndex + 1, 0, TIMBER_TEST_MAX_PIECE_COUNT);
    timber.width = 0;
    cutIndex++;
    scoreCountTicks = 1;
  }
  if (scoreCountTicks == 0 && input.isJustPressed) {
    isShowingIndicator = false;
    float cw = saw.x - timber.x;
    if (cw > 0) {
      play(SELECT);
      ASSIGN_ARRAY_ITEM(pieces, pieceIndex, Piece, p);
      vectorSet(&p->size, cw, 10);
      vectorSet(&p->pos, timber.x + p->size.x / 2, 20 + 10 / 2);
      vectorSet(&p->targetPos, 50, 40 + cutIndex * 15);
      p->isAlive = true;
      pieceIndex = wrap(pieceIndex + 1, 0, TIMBER_TEST_MAX_PIECE_COUNT);
      timber.x = saw.x;
      timber.width -= cw;
      cutIndex++;
    }
  }
  FOR_EACH(pieces, i) {
    ASSIGN_ARRAY_ITEM(pieces, i, Piece, p);
    SKIP_IS_NOT_ALIVE(p);
    if (distanceTo(&p->pos, VEC_XY(p->targetPos)) < 1) {
      vectorSet(&p->pos, VEC_XY(p->targetPos));
    } else {
      Vector diff;
      vectorSet(&diff, VEC_XY(p->targetPos));
      vectorAdd(&diff, -p->pos.x, -p->pos.y);
      vectorMul(&diff, 0.1);
      vectorAdd(&p->pos, VEC_XY(diff));
    }
    color = RED;
    box(VEC_XY(p->pos), VEC_XY(p->size));
  }
  if (scoreCountTicks > 0) {
    COUNT_IS_ALIVE(pieces, pc);
    scoreCountTicks += difficulty * (pc + 1) * 0.5;
    int c = clamp(floorf(scoreCountTicks / 20.0f), 0, fmaxf(cutCount, pc));
    TIMES(c, i) {
      color = BLACK;
      float y = 40 + i * 15;
      if (i == 0) {
        if (turnScore < 0) {
          color = RED;
        }
        text(intToChar(turnScore), 80, y);
      } else {
        float pw1 = i - 1 < pc ? pieces[i - 1].size.x : 0;
        float pw2 = i < pc ? pieces[i].size.x : 0;
        int p = (pw1 == 0 && pw2 == 0) || i > cutCount
                    ? 100
                    : floorf(fabsf(pw1 - pw2) / (pw1 + pw2) * 300);
        text("-", 74, y);
        text(intToChar(p), 80, y);
        if (i == scoreCountIndex) {
          play(HIT);
          turnScore -= p;
          scoreCountIndex++;
        }
      }
    }
  }
  if (saw.x > 160) {
    if (turnScore < 0) {
      play(EXPLOSION);
      gameOver();
    } else {
      score += turnScore;
      nextTimber();
    }
  }
  color = BLACK;
  if (turnIndex <= 3 && isShowingIndicator) {
    TIMES(cutCount - 1, i) {
      char indicatorText[2] = "^";
      text(indicatorText, timber.x + (timber.width / cutCount) * (i + 1), 35);
    }
    text("Cut here!", 32, 38);
  }
}

static void nextTimber() {
  play(POWER_UP);
  float tw = rnd(40, 80);
  timber.x = (100 - tw) / 2 + rnd(0, (100 - tw) / 3);
  timber.width = tw;
  cutCount = rndi(2, 5);
  turnScore = (cutCount - 1) * 100;
  cutIndex = 0;
  INIT_UNALIVED_ARRAY(pieces);
  saw.x = -30;
  saw.vx = (difficulty / sqrtf(cutCount)) * 2;
  scoreCountTicks = 0;
  scoreCountIndex = 1;
  turnIndex++;
  isShowingIndicator = true;
}

void addGameTimberTest() {
  addGame(title, description, characters, charactersCount, options, update);
}
