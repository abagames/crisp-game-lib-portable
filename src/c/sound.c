#include <math.h>
#include <stdbool.h>

#include "cglp.h"
#include "machineDependent.h"

#define QUANTIZED_TONE_PER_NOTE 2
#define QUANTIZED_DURATION (60 / tempo / QUANTIZED_TONE_PER_NOTE)

float nextTime;
bool isSoundEnabled = true;

void initSound() { nextTime = md_getAudioTime() + 1; }

void updateSound() {
  if (!isSoundEnabled) {
    return;
  }
  float ct = md_getAudioTime();
  if (ct > nextTime) {
    nextTime = ct;
  }
  if (nextTime < ct + 0.1) {
    nextTime += 60.0f / tempo;
    float d = 60.0f / tempo / 32;
    md_playTone(900, d, nextTime);
    md_playTone(700, d, nextTime + d);
  }
}

void playSoundEffect(int type) {
  if (!isSoundEnabled) {
    return;
  }
  float ct = md_getAudioTime();
  float qt = ceil(ct / QUANTIZED_DURATION) * QUANTIZED_DURATION;
  float d = 60.0f / tempo / 32;
  md_playTone(440, d, qt);
  md_playTone(540, d, qt + d);
  md_playTone(640, d, qt + d * 2);
  md_playTone(740, d, qt + d * 3);
}
