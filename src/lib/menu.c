#include "menu.h"

#include "cglp.h"

#define MAX_GAME_COUNT 16
#define KEY_REPEAT_DURATION 30
int gameCount = 0;
static Game games[MAX_GAME_COUNT];
static int gameIndex = 1;
static int keyRepeatTicks = 0;

static void update() {
  color = BLUE;
  text("[A]      [B]", 3, 3);
  color = BLACK;
  text("   Select   Down", 3, 3);
  if (input.b.isPressed || input.down.isPressed || input.up.isPressed) {
    keyRepeatTicks++;
  } else {
    keyRepeatTicks = 0;
  }
  if (input.b.isJustPressed || input.down.isJustPressed ||
      (keyRepeatTicks > KEY_REPEAT_DURATION &&
       (input.b.isPressed || input.down.isPressed))) {
    gameIndex++;
    if (keyRepeatTicks > KEY_REPEAT_DURATION) {
      keyRepeatTicks = KEY_REPEAT_DURATION / 3 * 2;
    }
  }
  if (input.up.isJustPressed ||
      (keyRepeatTicks > KEY_REPEAT_DURATION && input.up.isPressed)) {
    gameIndex--;
    if (keyRepeatTicks > KEY_REPEAT_DURATION) {
      keyRepeatTicks = KEY_REPEAT_DURATION / 3 * 2;
    }
  }
  gameIndex = wrap(gameIndex, 1, gameCount);
  color = BLACK;
  for (int i = 0; i < gameCount; i++) {
    float y = i * 6;
    if (i == gameIndex) {
      color = BLUE;
      text(">", 3, y + 3);
      color = BLACK;
    }
    text(games[i].title, 9, y + 3);
  }
  if (input.a.isJustPressed) {
    restartGame(gameIndex);
  }
}

void addGame(char *title, char *description,
             char (*characters)[CHARACTER_WIDTH][CHARACTER_HEIGHT + 1],
             int charactersCount, Options options, void (*update)(void)) {
  if (gameCount >= MAX_GAME_COUNT) {
    return;
  }
  Game *g = &games[gameCount];
  g->title = title;
  g->description = description;
  g->characters = characters;
  g->charactersCount = charactersCount;
  g->options = options;
  g->update = update;
  gameCount++;
}

Game getGame(int index) { return games[index]; }

void addMenu() {
  Options o = {
      .viewSizeX = 100, .viewSizeY = 100, .soundSeed = 0, .isDarkColor = false};
  addGame("", "", NULL, 0, o, update);
}
