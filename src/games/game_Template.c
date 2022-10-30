#include "cglp.h"

static char *title = "";
static char *description = "";

#define CS static char characters[][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1]
CS = {};
static int charactersCount = 0;

static Options options = {
    .viewSizeX = 100, .viewSizeY = 100, .soundSeed = 0, .isDarkColor = false};

static void update() {
  if (!ticks) {
  }
}

void addGame_Template() {
  addGame(title, description, characters, charactersCount, options, update);
}
