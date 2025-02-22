#include "cglp.h"

static char *title = "GRAPPLING H";
static char *description = "\n[Tap]\n Release hook\n Hold anchor";

#define CS static char characters[][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1]
CS = {{
    "      ",
    "      ",
    "      ",
    "      ",
    "      ",
    "      ",
}};
static int charactersCount = 0;

static Options options = {
    .viewSizeX = 100, .viewSizeY = 150, .soundSeed = 5, .isDarkColor = false};

typedef struct {
  Vector pos;
  Vector vel;
  Vector *attachedAnchor;
  bool isAlive;
} Player;
typedef struct {
  Vector pos;
  bool isAlive;
} AnchorPoint;

#define MAX_ANCHOR_POINTS 32
static Player player;
static AnchorPoint anchorPoints[MAX_ANCHOR_POINTS];
static int anchorPointIndex;
static float scrolledDistanceY;
static float scrolledDistanceYForScore;
static bool inputIsReleased;
static bool inputIsJustPressed;
static float nextAnchorDistance = 20;
static float playerRadius = 4;
static float anchorRadius = 1;

static void addAnchorPoint(float x, float y) {
  ASSIGN_ARRAY_ITEM(anchorPoints, anchorPointIndex, AnchorPoint, ap);
  ap->pos.x = x;
  ap->pos.y = y;
  ap->isAlive = true;
  anchorPointIndex = wrap(anchorPointIndex + 1, 0, MAX_ANCHOR_POINTS);
}

static void update() {
  if (!ticks) {
    INIT_UNALIVED_ARRAY(anchorPoints);
    anchorPointIndex = 0;
    addAnchorPoint(50, 0);

    player.pos = (Vector){30, 30};
    player.vel = (Vector){0, 0};
    player.attachedAnchor = &anchorPoints[0].pos;
    player.isAlive = true;

    scrolledDistanceY = 0;
    scrolledDistanceYForScore = 0;
    inputIsReleased = false;
    inputIsJustPressed = false;
  }

  inputIsJustPressed = input.isJustPressed;
  float scrollY = 0.1 * difficulty;
  if (player.pos.y < 75) {
    scrollY += (75 - player.pos.y) * 0.1;
  }
  scrolledDistanceY += scrollY;
  scrolledDistanceYForScore += scrollY;
  while (scrolledDistanceY > nextAnchorDistance) {
    scrolledDistanceY -= nextAnchorDistance;
    addAnchorPoint(rnd(10, 90), -5 + scrolledDistanceY);
  }

  AnchorPoint *attachableAnchor = NULL;
  float attachableDist = 25;
  FOR_EACH(anchorPoints, i) {
    ASSIGN_ARRAY_ITEM(anchorPoints, i, AnchorPoint, ap);
    SKIP_IS_NOT_ALIVE(ap);
    float dist = distanceTo(&player.pos, VEC_XY(ap->pos));
    if (player.attachedAnchor == NULL && dist < attachableDist) {
      attachableAnchor = ap;
      attachableDist = dist;
    }
  }
  FOR_EACH(anchorPoints, i) {
    ASSIGN_ARRAY_ITEM(anchorPoints, i, AnchorPoint, ap);
    SKIP_IS_NOT_ALIVE(ap);
    ap->pos.y += scrollY;
    color = (ap == attachableAnchor) ? RED : BLACK;
    arc(VEC_XY(ap->pos), anchorRadius, 0, M_PI * 2);
    if (ap == attachableAnchor) {
      color = RED;
      line(VEC_XY(player.pos), VEC_XY(ap->pos));
      if (inputIsReleased && input.isPressed) {
        play(COIN);
        player.attachedAnchor = &ap->pos;
        if (scrolledDistanceYForScore > 0) {
          addScore(ceil(sqrt(scrolledDistanceYForScore * scrolledDistanceYForScore) * 0.1), VEC_XY(ap->pos));
        }
        inputIsJustPressed = false;
      }
    }
    ap->isAlive = ap->pos.y <= 155;
  }

  color = BLUE;
  vectorAdd(&player.pos, VEC_XY(player.vel));
  vectorMul(&player.vel, 1 - fabs(vectorLength(&player.vel)) * 0.001);
  player.vel.y += 0.1;
  player.pos.y += scrollY;
  if ((player.pos.x < 0 && player.vel.x < 0) || (player.pos.x > 100 && player.vel.x > 0)) {
    play(HIT);
    player.vel.x *= -0.8;
  }
  if (player.attachedAnchor != NULL) {
    if (player.attachedAnchor->y > 150) {
      play(EXPLOSION);
      gameOver();
    }
    float dist = distanceTo(&player.pos, player.attachedAnchor->x, player.attachedAnchor->y);
    float angle = angleTo(&player.pos, player.attachedAnchor->x, player.attachedAnchor->y);
    float force = dist;
    addWithAngle(&player.vel, angle, force * 0.01);
    line(VEC_XY(player.pos), player.attachedAnchor->x, player.attachedAnchor->y);
    if (inputIsJustPressed) {
      play(JUMP);
      vectorSet(player.attachedAnchor, 0, 999);
      player.attachedAnchor = NULL;
      addWithAngle(&player.vel, angle, force * 0.2);
      scrolledDistanceYForScore = 0;
      inputIsReleased = false;
    }
  } else {
    if (player.pos.y > 150 && player.vel.y > 0) {
      play(EXPLOSION);
      gameOver();
    }
  }
  arc(VEC_XY(player.pos), playerRadius, 0, M_PI * 2);
  if (input.isJustReleased) {
    inputIsReleased = true;
  }
}

void addGameGrapplingH() {
  addGame(title, description, characters, charactersCount, options, update);
}
