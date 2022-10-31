#include "menu.h"

#include "cglp.h"

#define MAX_GAME_COUNT 16
int gameCount = 0;
static Game games[MAX_GAME_COUNT];
static int gameIndex = 1;

static void update() {
  color = BLACK;
  text("[A]Select[B]Down", 3, 3);
  if (input.b.isJustPressed || input.down.isJustPressed) {
    gameIndex++;
  }
  if (input.up.isJustPressed) {
    gameIndex--;
  }
  gameIndex = wrap(gameIndex, 1, gameCount);
  for (int i = 0; i < gameCount; i++) {
    color = BLACK;
    float y = i * 6;
    if (i == gameIndex) {
      rect(0, y, 100, 6);
      color = WHITE;
    }
    text(games[i].title, 3, y + 3);
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
