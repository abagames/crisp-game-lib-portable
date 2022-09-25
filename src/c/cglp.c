#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

#include <math.h>
#include <stdbool.h>
#include <string.h>

#include "cglp.h"
#include "machineDependent.h"
#include "sound.h"
#include "textPattern.h"

Input input;
int ticks;
float tempo = 120;

// Collision
typedef struct {
  int rectIndex;
  int textIndex;
  int characterIndex;
  Vector pos;
  Vector size;
} HitBox;

Options options;

#define MAX_HIT_BOX_COUNT 256
HitBox hitBoxes[MAX_HIT_BOX_COUNT];
int hitBoxesIndex;
HitBox textHitBoxes[TEXT_PATTERN_COUNT];
HitBox characterHitBoxes[MAX_CHARACTER_PATTERN_COUNT];

void initCollision(Collision *collision) {
  for (int i = 0; i < COLOR_COUNT; i++) {
    collision->isColliding.rect[i] = false;
  }
  for (int i = '!'; i <= '~'; i++) {
    collision->isColliding.text[i] = false;
  }
  for (int i = 'a'; i <= 'z'; i++) {
    collision->isColliding.character[i] = false;
  }
}

bool testCollision(HitBox r1, HitBox r2) {
  int ox = r2.pos.x - r1.pos.x;
  int oy = r2.pos.y - r1.pos.y;
  return -r2.size.x < ox && ox < r1.size.x && -r2.size.y < oy && oy < r1.size.y;
}

void checkHitBox(Collision *cl, HitBox hitBox) {
  for (int i = 0; i < hitBoxesIndex; i++) {
    HitBox hb = hitBoxes[i];
    if (testCollision(hb, hitBox)) {
      if (hb.rectIndex >= 0) {
        cl->isColliding.rect[hb.rectIndex] = true;
      }
      if (hb.textIndex >= 0) {
        cl->isColliding.text[hb.textIndex] = true;
      }
      if (hb.characterIndex >= 0) {
        cl->isColliding.character[hb.characterIndex] = true;
      }
    }
  }
}

void setTextCharacterHitBoxes(
    HitBox *hb, char grid[CHARACTER_HEIGHT][CHARACTER_WIDTH + 1]) {
  hb->pos.x = -CHARACTER_WIDTH / 2;
  hb->pos.y = -CHARACTER_HEIGHT / 2;
  hb->size.x = CHARACTER_WIDTH;
  hb->size.y = CHARACTER_HEIGHT;
}

void setTextHitBoxes() {
  for (int i = 0; i < TEXT_PATTERN_COUNT; i++) {
    setTextCharacterHitBoxes(&textHitBoxes[i], textPatterns[i]);
  }
}

void setCharacterHitBoxes() {
  for (int i = 0; i < charactersCount; i++) {
    setTextCharacterHitBoxes(&characterHitBoxes[i], characters[i]);
  }
}

// Initialize
EMSCRIPTEN_KEEPALIVE
void initGame() {
  md_setTexts(textPatterns, TEXT_PATTERN_COUNT);
  md_setCharacters(characters, charactersCount);
  setTextHitBoxes();
  setCharacterHitBoxes();
  options.viewSize.x = 100;
  options.viewSize.y = 100;
  initSound();
  ticks = 0;
}

void initFrame() { hitBoxesIndex = 0; }

EMSCRIPTEN_KEEPALIVE
void updateFrame() {
  initFrame();
  update();
  updateSound();
  ticks++;
}

EMSCRIPTEN_KEEPALIVE
void setInput(bool isPressed, bool isJustPressed, bool isJustReleased) {
  input.isPressed = isPressed;
  input.isJustPressed = isJustPressed;
  input.isJustReleased = isJustReleased;
}

#define MAX_DRAWING_HIT_BOXES_COUNT 64
HitBox drawingHitBoxes[MAX_DRAWING_HIT_BOXES_COUNT];
int drawingHitBoxesIndex;

void beginAddingRects() { drawingHitBoxesIndex = 0; }

void addRect(bool isAlignCenter, float x, float y, float w, float h,
             Collision *hitCollision) {
  if (isAlignCenter) {
    x -= w / 2;
    y -= h / 2;
  }
  HitBox hb;
  hb.rectIndex = BLACK;
  hb.textIndex = hb.characterIndex = -1;
  hb.pos.x = floor(x);
  hb.pos.y = floor(y);
  hb.size.x = floor(w);
  hb.size.y = floor(h);
  checkHitBox(hitCollision, hb);
  if (drawingHitBoxesIndex < MAX_DRAWING_HIT_BOXES_COUNT) {
    drawingHitBoxes[drawingHitBoxesIndex] = hb;
    drawingHitBoxesIndex++;
  }
  md_rect(x, y, w, h);
}

void addHitBox(HitBox hb) {
  if (hitBoxesIndex < MAX_HIT_BOX_COUNT) {
    hitBoxes[hitBoxesIndex] = hb;
    hitBoxesIndex++;
  } else {
    consoleLog("too many hit boxes");
  }
}

void endAddingRects() {
  for (int i = 0; i < drawingHitBoxesIndex; i++) {
    addHitBox(drawingHitBoxes[i]);
  }
}

Collision rect(float x, float y, float w, float h) {
  Collision hitCollision;
  initCollision(&hitCollision);
  beginAddingRects();
  addRect(false, x, y, w, h, &hitCollision);
  endAddingRects();
  return hitCollision;
}

float thickness = 3;

void drawLine(float x, float y, float ox, float oy, Collision *hitCollision) {
  int t = floor(thickness);
  if (t < 3) {
    t = 3;
  } else if (t > 10) {
    t = 10;
  }
  float lx = fabs(ox);
  float ly = fabs(oy);
  float rn = ceil(lx > ly ? lx / t : ly / t);
  if (rn < 3) {
    rn = 3;
  } else if (rn > 99) {
    rn = 99;
  }
  ox /= (rn - 1);
  oy /= (rn - 1);
  for (int i = 0; i < rn; i++) {
    addRect(true, x, y, thickness, thickness, hitCollision);
    x += ox;
    y += oy;
  }
}

Collision line(float x1, float y1, float x2, float y2) {
  Collision hitCollision;
  initCollision(&hitCollision);
  beginAddingRects();
  drawLine(x1, y1, x2 - x1, y2 - y1, &hitCollision);
  endAddingRects();
  return hitCollision;
}

Collision arc(float centerX, float centerY, float radius, float angleFrom,
              float angleTo) {
  Collision hitCollision;
  initCollision(&hitCollision);
  beginAddingRects();
  float af, ao;
  if (angleFrom > angleTo) {
    af = angleTo;
    ao = angleFrom - angleTo;
  } else {
    af = angleFrom;
    ao = angleTo - angleFrom;
  }
  if (ao < 0.01) {
    return hitCollision;
  }
  if (ao < 0) {
    ao = 0;
  } else if (ao > M_PI * 2) {
    ao = M_PI * 2;
  }
  int lc = ceil(ao * sqrt(radius * 0.25));
  if (lc < 1) {
    lc = 1;
  } else if (lc > 36) {
    lc = 36;
  }
  float ai = ao / lc;
  float a = af;
  float p1x = radius * cos(a) + centerX;
  float p1y = radius * sin(a) + centerY;
  float p2x, p2y;
  float ox, oy;
  for (int i = 0; i < lc; i++) {
    a += ai;
    p2x = radius * cos(a) + centerX;
    p2y = radius * sin(a) + centerY;
    ox = p2x - p1x;
    oy = p2y - p1y;
    drawLine(p1x, p1y, ox, oy, &hitCollision);
    p1x = p2x;
    p1y = p2y;
  }
  endAddingRects();
  return hitCollision;
}

Collision text(char *msg, int x, int y) {
  Collision hitCollision;
  initCollision(&hitCollision);
  int ml = strlen(msg);
  for (int i = 0; i < ml; i++) {
    if (msg[i] >= '!' && msg[i] <= '~') {
      HitBox *thb = &textHitBoxes[msg[i] - '!'];
      HitBox hb;
      hb.textIndex = msg[i];
      hb.rectIndex = hb.characterIndex = -1;
      hb.pos.x = floor(x + thb->pos.x);
      hb.pos.y = floor(y + thb->pos.y);
      hb.size.x = thb->size.x;
      hb.size.y = thb->size.y;
      checkHitBox(&hitCollision, hb);
      addHitBox(hb);
      md_text(msg[i], x, y);
    }
    x += 6;
  }
  return hitCollision;
}

Collision character(char *msg, float x, float y) {
  Collision hitCollision;
  initCollision(&hitCollision);
  int ml = strlen(msg);
  for (int i = 0; i < ml; i++) {
    if (msg[i] >= 'a' && msg[i] <= 'z') {
      HitBox *thb = &characterHitBoxes[msg[i] - 'a'];
      HitBox hb;
      hb.characterIndex = msg[i];
      hb.rectIndex = hb.textIndex = -1;
      hb.pos.x = floor(x + thb->pos.x);
      hb.pos.y = floor(y + thb->pos.y);
      hb.size.x = thb->size.x;
      hb.size.y = thb->size.y;
      checkHitBox(&hitCollision, hb);
      addHitBox(hb);
      md_character(msg[i], x, y);
    }
    x += 6;
  }
  return hitCollision;
}

void play(int type) { playSoundEffect(type); }

void enableSound() { isSoundEnabled = true; }

void disableSound() {
  isSoundEnabled = false;
  md_stopTone();
}

void toggleSound() {
  if (isSoundEnabled) {
    disableSound();
  } else {
    enableSound();
  }
}

void consoleLog(char *msg) { md_consoleLog(msg); }
