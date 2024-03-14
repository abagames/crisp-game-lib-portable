#include "cglp.h"

static char* title = "FRACAVE";
static char* description = "[Hold]\n Accelerate";

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
    .viewSizeX = 100, .viewSizeY = 150, .soundSeed = 4, .isDarkColor = false};

typedef struct {
  Vector pos;
  float angle;
  float speed;
  float noReflectionDistance;
} Player;
static Player player;

typedef struct {
  float x1;
  float y1;
  float x2;
  float y2;
  bool isAlive;
} Wall;
#define FRACAVE_MAX_WALL_COUNT 100
static Wall walls[FRACAVE_MAX_WALL_COUNT];
static int wallIndex;

static float wallSpeed;
static float wallInterval;

static void generateFractalWall(float x1, float y1, float x2, float y2,
                                int depth) {
  if (depth == 0) {
    walls[wallIndex].x1 = x1;
    walls[wallIndex].y1 = y1;
    walls[wallIndex].x2 = x2;
    walls[wallIndex].y2 = y2;
    walls[wallIndex].isAlive = true;
    wallIndex = wrap(wallIndex + 1, 0, FRACAVE_MAX_WALL_COUNT);
    return;
  }
  float midX = (x1 + x2) / 2;
  float midY = (y1 + y2) / 2;
  float offsetX = rnd(-10, 10);
  float offsetY = rnd(-5, 5);
  generateFractalWall(x1, y1, midX + offsetX, midY + offsetY, depth - 1);
  generateFractalWall(midX + offsetX, midY + offsetY, x2, y2, depth - 1);
}

static void update() {
  if (!ticks) {
    vectorSet(&player.pos, 40, 75);
    player.angle = 0;
    player.speed = 1;
    player.noReflectionDistance = 0;
    INIT_UNALIVED_ARRAY(walls);
    wallIndex = 0;
    generateFractalWall(20, 0, 20, 150, 3);
    generateFractalWall(80, 0, 80, 150, 3);
    wallSpeed = 1;
    wallInterval = 150;
  }
  wallInterval += wallSpeed;
  if (wallInterval >= 150) {
    wallInterval = 0;
    generateFractalWall(20, -150, 20, 0, 3);
    generateFractalWall(80, -150, 80, 0, 3);
  }
  if (input.isJustPressed) {
    play(CLICK);
  }
  if (input.isPressed) {
    player.speed += (2 - player.speed) * 0.3f;
  } else {
    player.speed += (0.1f - player.speed) * 0.3f;
  }
  addWithAngle(&player.pos, player.angle, player.speed * difficulty);
  player.noReflectionDistance -= player.speed * difficulty;
  FOR_EACH(walls, i) {
    ASSIGN_ARRAY_ITEM(walls, i, Wall, wall);
    SKIP_IS_NOT_ALIVE(wall);
    wall->y1 += wallSpeed;
    wall->y2 += wallSpeed;
    wall->isAlive = wall->y1 < 170;
  }
  color = CYAN;
  bar(VEC_XY(player.pos), 7, player.angle);
  color = BLACK;
  FOR_EACH(walls, i) {
    ASSIGN_ARRAY_ITEM(walls, i, Wall, wall);
    SKIP_IS_NOT_ALIVE(wall);
    if (line(wall->x1, wall->y1, wall->x2, wall->y2).isColliding.rect[CYAN] &&
        player.noReflectionDistance < 0) {
      Vector wallVector = {wall->x2 - wall->x1, wall->y2 - wall->y1};
      Vector wallNormal = {-wallVector.y, wallVector.x};
      vectorMul(&wallNormal, 1.0f / vectorLength(&wallNormal));
      Vector playerVector = {cosf(player.angle), sinf(player.angle)};
      float dot = playerVector.x * wallNormal.x + playerVector.y * wallNormal.y;
      Vector reflectVector = {playerVector.x - 2 * dot * wallNormal.x,
                              playerVector.y - 2 * dot * wallNormal.y};
      player.angle = atan2f(reflectVector.y, reflectVector.x);
      player.speed = 0.2f;
      addScore(1, VEC_XY(player.pos));
      addWithAngle(&player.pos, player.angle, 7);
      play(HIT);
      player.noReflectionDistance = 9;
    }
  }
  if (player.pos.x < 0 || player.pos.x > 100 || player.pos.y < 0 ||
      player.pos.y > 150) {
    play(EXPLOSION);
    gameOver();
  }
  wallSpeed = difficulty;
}

void addGameFracave() {
  addGame(title, description, characters, charactersCount, options, update);
}
