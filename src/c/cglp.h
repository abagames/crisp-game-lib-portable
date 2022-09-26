#ifndef CGLP_H
#define CGLP_H

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC extern
#endif

#include <stdbool.h>

#define FPS 60

#define COLOR_COUNT 8
#define TRANSPARENT -1
#define WHITE 0
#define BLACK 7

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

typedef struct {
  float x;
  float y;
} Vector;

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
} Input;

typedef struct {
  Vector viewSize;
} Options;

EXTERNC Options options;
EXTERNC float tempo;
EXTERNC int ticks;
EXTERNC Input input;
EXTERNC float thickness;
EXTERNC Collision rect(float x, float y, float w, float h);
EXTERNC Collision line(float x1, float y1, float x2, float y2);
EXTERNC Collision arc(float centerX, float centerY, float radius,
                      float angleFrom, float angleTo);
EXTERNC Collision text(char *msg, int x, int y);
EXTERNC Collision character(char *msg, float x, float y);
EXTERNC void play(int type);
EXTERNC void enableSound();
EXTERNC void disableSound();
EXTERNC void toggleSound();
EXTERNC void consoleLog(char *msg);

EXTERNC void setInput(bool isPressed, bool isJustPressed, bool isJustReleased);
EXTERNC void initGame();
EXTERNC void updateFrame();

EXTERNC char characters[][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1];
EXTERNC int charactersCount;
EXTERNC void update();

#endif