/// \cond
#ifndef CGLP_H
#define CGLP_H

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC extern
#endif

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

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

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
/// \endcond

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
  Vector pos;
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

/// \cond
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
EXTERNC float rnd(float low, float high);
EXTERNC int rndi(int low, int high);
EXTERNC void gameOver();
EXTERNC void particle(float x, float y, float count, float speed, float angle,
                      float angleWidth);

EXTERNC float clamp(float v, float low, float high);
EXTERNC float wrap(float v, float low, float high);
EXTERNC char *intToChar(int v);
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

EXTERNC void initGame();
EXTERNC void setButtonState(bool left, bool right, bool up, bool down, bool b,
                            bool a);
EXTERNC void updateFrame();
/// \endcond

//! Iterate over an `array` with variable `index`
#define FOR_EACH(array, index) \
  for (int index = 0; index < sizeof(array) / sizeof(array[0]); index++)
//! Assign the `index` th item in the `array` to an `item` variable of `type`
#define ASSIGN_ARRAY_ITEM(array, index, type, item) type *item = &array[index]
//! Skip (continue) if the member `isAlive` of variable `item` is false.
#define SKIP_IS_NOT_ALIVE(item) \
  if (!item->isAlive) continue
//! Iterate `count` times with variable `index`
#define TIMES(count, index) for (int index = 0; index < count; index++)
//! Count the number of items in the array for which the `isAlive` member is
//! true and assigns it to a variable defined as the int variable `counter`
#define COUNT_IS_ALIVE(array, counter)                           \
  int counter = 0;                                               \
  do {                                                           \
    for (int i = 0; i < sizeof(array) / sizeof(array[0]); i++) { \
      if (array[i].isAlive) {                                    \
        counter++;                                               \
      }                                                          \
    }                                                            \
  } while (0)
//! Set the `isAlive` member to false for all items in the `array`
#define INIT_UNALIVED_ARRAY(array)                               \
  do {                                                           \
    for (int i = 0; i < sizeof(array) / sizeof(array[0]); i++) { \
      array[i].isAlive = false;                                  \
    }                                                            \
  } while (0)
//! Return 1 or -1 randomly
#define RNDPM() (rndi(0, 2) * 2 - 1)

#include "menu.h"

#endif
