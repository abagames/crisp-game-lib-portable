#ifndef CGLP_H
#define CGLP_H

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC extern
#endif

#include <stdbool.h>

#include "machineDependent.h"
#include "vector.h"

#define FPS 60

#define COLOR_COUNT 15
#define TRANSPARENT -1
#define WHITE 0
#define RED 1
#define GREEN 2
#define YELLOW 3
#define BLUE 4
#define PURPLE 5
#define CYAN 6
#define BLACK 7
#define LIGHT_RED 8
#define LIGHT_GREEN 9
#define LIGHT_YELLOW 10
#define LIGHT_BLUE 11
#define LIGHT_PURPLE 12
#define LIGHT_CYAN 13
#define LIGHT_BLACK 14

#define CHARACTER_WIDTH 6
#define CHARACTER_HEIGHT 6

#define SOUND_EFFECT_TYPE_COUNT 9
#define COIN 0
#define LASER 1
#define EXPLOSION 2
#define POWER_UP 3
#define HIT 4
#define JUMP 5
#define SELECT 6
#define RANDOM 7
#define CLICK 8

#define TEXT_PATTERN_COUNT 94
#define MAX_CHARACTER_PATTERN_COUNT 26
#define ASCII_CHARACTER_COUNT 127
#define MAX_CACHED_CHARACTER_PATTERN_COUNT 128

typedef struct {
  bool black;
} RectCollision;

typedef struct {
  bool rect[COLOR_COUNT];
  bool text[ASCII_CHARACTER_COUNT];
  bool character[ASCII_CHARACTER_COUNT];
} Collisions;

typedef struct {
  Collisions isColliding;
} Collision;

typedef struct {
  bool isPressed;
  bool isJustPressed;
  bool isJustReleased;
} ButtonState;

typedef struct {
  ButtonState left;
  ButtonState right;
  ButtonState up;
  ButtonState down;
  ButtonState b;
  ButtonState a;
  bool isPressed;
  bool isJustPressed;
  bool isJustReleased;
} Input;

typedef struct {
  int viewSizeX;
  int viewSizeY;
  int soundSeed;
  bool isDarkColor;
} Options;

typedef struct {
  bool isMirrorX;
  bool isMirrorY;
  int rotation;
} CharacterOptions;

EXTERNC int ticks;
EXTERNC float score;
EXTERNC float difficulty;
EXTERNC int color;
EXTERNC float thickness;
EXTERNC float barCenterPosRatio;
EXTERNC CharacterOptions characterOptions;
EXTERNC bool hasCollision;
EXTERNC float tempo;
EXTERNC Input input;
EXTERNC Input currentInput;
EXTERNC bool isInMenu;
EXTERNC Collision rect(float x, float y, float w, float h);
EXTERNC Collision box(float x, float y, float w, float h);
EXTERNC Collision line(float x1, float y1, float x2, float y2);
EXTERNC Collision bar(float x, float y, float length, float angle);
EXTERNC Collision arc(float centerX, float centerY, float radius,
                      float angleFrom, float angleTo);
EXTERNC Collision text(char *msg, float x, float y);
EXTERNC Collision character(char *msg, float x, float y);
EXTERNC void play(int type);
EXTERNC void addScore(float value, float x, float y);
EXTERNC float rnd(float high, float low);
EXTERNC int rndi(int high, int low);
EXTERNC void gameOver();
EXTERNC void particle(float x, float y, float count, float speed, float angle,
                      float angleWidth);

EXTERNC float clamp(float v, float low, float high);
EXTERNC float wrap(float v, float low, float high);
EXTERNC void consoleLog(char *format, ...);
EXTERNC void enableSound();
EXTERNC void disableSound();
EXTERNC void toggleSound();
EXTERNC void goToMenu();
EXTERNC void restartGame(int gameIndex);

typedef struct {
  unsigned char r;
  unsigned char g;
  unsigned char b;
} ColorRgb;

EXTERNC int currentColorIndex;
EXTERNC ColorRgb colorRgbs[COLOR_COUNT];
EXTERNC int getHashFromString(char *str);

// initGame() should be called at game initialization time
EXTERNC void initGame();
// SetButtonState() and updateFrame() should be called once per frame
EXTERNC void setButtonState(bool left, bool right, bool up, bool down, bool b,
                            bool a);
EXTERNC void updateFrame();

#define FOR_EACH(array, index) \
  for (int index = 0; index < sizeof(array) / sizeof(array[0]); index++)
#define ASSIGN_ARRAY_ITEM(array, index, type, item) type *item = &array[index]
#define SKIP_IS_NOT_ALIVE(item) \
  if (!item->isAlive) continue
#define TIMES(count, index) for (int index = 0; index < count; index++)
#define COUNT_IS_ALIVE(array, counter)                           \
  int counter = 0;                                               \
  do {                                                           \
    for (int i = 0; i < sizeof(array) / sizeof(array[0]); i++) { \
      if (array[i].isAlive) {                                    \
        counter++;                                               \
      }                                                          \
    }                                                            \
  } while (0)
#define INIT_UNALIVED_ARRAY(array)                               \
  do {                                                           \
    for (int i = 0; i < sizeof(array) / sizeof(array[0]); i++) { \
      array[i].isAlive = false;                                  \
    }                                                            \
  } while (0)
#define RNDPM (rndi(0, 2) * 2 - 1)

#endif

#include "menu.h"
