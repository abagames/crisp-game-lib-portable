#include "sound.h"

#include <math.h>
#include <stdbool.h>

#include "cglp.h"
#include "machineDependent.h"
#include "random.h"

#define QUANTIZED_TONE_PER_NOTE 2
#define QUANTIZED_DURATION (60.0f / tempo / QUANTIZED_TONE_PER_NOTE)

float nextTime;
bool isSoundEnabled = true;

typedef struct {
  float freq;
  float duration;
  float when;
} Note;

#define MAX_SOUND_EFFECT_NOTE_LENGTH 32
#define BASE_NOTE_DURATION (60.0f / tempo / 32)
Note soundEffects[SOUND_EFFECT_TYPE_COUNT][MAX_SOUND_EFFECT_NOTE_LENGTH];
Random random;

float midiNoteToFreq(int midiNote) {
  return 440 * pow(2, (float)(midiNote - 69) / 12);
}

void addNotes(Note *ns, int count, int when, int from, int to,
              float amplitudeFrom, float amplitudeTo) {
  float mn = from;
  float mo = (float)(to - from) / (count - 1);
  float an = amplitudeFrom;
  float ao = (float)(amplitudeTo - amplitudeFrom) / (count - 1);
  for (int i = 0; i < count; i++) {
    Note *n = &ns[i];
    n->freq = midiNoteToFreq((int)(mn + getRandom(&random, -an, an)));
    n->duration = BASE_NOTE_DURATION;
    n->when = when * BASE_NOTE_DURATION;
    mn += mo;
    an += ao;
    when++;
  }
}

void coin(Note *ns) {
  int i = 0;
  int w = 0;
  int d = getIntRandom(&random, 4, 8);
  ns[i].freq = midiNoteToFreq(getIntRandom(&random, 70, 80));
  ns[i].when = w * BASE_NOTE_DURATION;
  ns[i].duration = d * BASE_NOTE_DURATION;
  i++;
  w += d;
  d = getIntRandom(&random, 7, 15);
  ns[i].freq = midiNoteToFreq(getIntRandom(&random, 85, 95));
  ns[i].when = w * BASE_NOTE_DURATION;
  ns[i].duration = d * BASE_NOTE_DURATION;
  i++;
  ns[i].freq = -1;
}

void laser(Note *ns) {
  int i = 0;
  int w = 0;
  int d = getIntRandom(&random, 9, 19);
  addNotes(&ns[i], d, w, getIntRandom(&random, 75, 95),
           getIntRandom(&random, 45, 65), getIntRandom(&random, 5, 9), 0);
  i += d;
  ns[i].freq = -1;
}

void explosion(Note *ns) {
  int i = 0;
  int w = 0;
  int d = getIntRandom(&random, 5, 12);
  addNotes(&ns[i], d, w, getIntRandom(&random, 70, 90),
           getIntRandom(&random, 50, 60), getIntRandom(&random, 5, 15),
           getIntRandom(&random, 15, 25));
  i++;
  w += d;
  d = getIntRandom(&random, 12, 20);
  addNotes(&ns[i], d, w, getIntRandom(&random, 50, 70),
           getIntRandom(&random, 30, 50), getIntRandom(&random, 15, 25),
           getIntRandom(&random, 5, 15));
  i += d;
  ns[i].freq = -1;
}

void powerUp(Note *ns) {
  int i = 0;
  int w = 0;
  int d = getIntRandom(&random, 2, 5);
  addNotes(&ns[i], d, w, getIntRandom(&random, 75, 85),
           getIntRandom(&random, 65, 75), 0, 0);
  i += d;
  w += d;
  d = getIntRandom(&random, 6, 9);
  addNotes(&ns[i], d, w, getIntRandom(&random, 65, 75),
           getIntRandom(&random, 85, 95), 0, 0);
  i += d;
  w += d;
  d = getIntRandom(&random, 3, 6);
  addNotes(&ns[i], d, w, getIntRandom(&random, 85, 95),
           getIntRandom(&random, 75, 85), 0, 0);
  i += d;
  w += d;
  d = getIntRandom(&random, 6, 9);
  addNotes(&ns[i], d, w, getIntRandom(&random, 75, 85),
           getIntRandom(&random, 95, 105), 0, 0);

  i += d;
  ns[i].freq = -1;
}

void hit(Note *ns) {
  int i = 0;
  int w = 0;
  int d = getIntRandom(&random, 5, 9);
  addNotes(&ns[i], d, w, getIntRandom(&random, 70, 80),
           getIntRandom(&random, 60, 70), getIntRandom(&random, 0, 10),
           getIntRandom(&random, 0, 10));
  i += d;
  ns[i].freq = -1;
}

void jump(Note *ns) {
  int i = 0;
  int w = 0;
  int d = getIntRandom(&random, 2, 5);
  ns[i].freq = midiNoteToFreq(getIntRandom(&random, 70, 80));
  ns[i].when = w * BASE_NOTE_DURATION;
  ns[i].duration = d * BASE_NOTE_DURATION;
  i++;
  w += d;
  d = getIntRandom(&random, 9, 19);
  addNotes(&ns[i], d, w, getIntRandom(&random, 55, 70),
           getIntRandom(&random, 80, 95), 0, 0);
  i += d;
  ns[i].freq = -1;
}

void select(Note *ns) {
  int i = 0;
  int w = 0;
  int d = getIntRandom(&random, 2, 4);
  int f = getIntRandom(&random, 60, 90);
  ns[i].freq = midiNoteToFreq(f);
  ns[i].when = w * BASE_NOTE_DURATION;
  ns[i].duration = ceil(d * 0.7) * BASE_NOTE_DURATION;
  i++;
  w += d;
  d = getIntRandom(&random, 4, 9);
  f += getIntRandom(&random, -2, 5);
  ns[i].freq = midiNoteToFreq(f);
  ns[i].when = w * BASE_NOTE_DURATION;
  ns[i].duration = d * BASE_NOTE_DURATION;
  i++;
  ns[i].freq = -1;
}

void randomSe(Note *ns) {
  int i = 0;
  int w = 0;
  int d = getIntRandom(&random, 3, 15);
  addNotes(&ns[i], d, w, getIntRandom(&random, 30, 99),
           getIntRandom(&random, 30, 99), getIntRandom(&random, 0, 20),
           getIntRandom(&random, 0, 20));
  i += d;
  w += d;
  d = getIntRandom(&random, 3, 15);
  addNotes(&ns[i], d, w, getIntRandom(&random, 30, 99),
           getIntRandom(&random, 30, 99), getIntRandom(&random, 0, 20),
           getIntRandom(&random, 0, 20));
  i += d;
  ns[i].freq = -1;
}

void click(Note *ns) {
  int i = 0;
  int w = 0;
  int d = getIntRandom(&random, 2, 6);
  ns[i].freq = midiNoteToFreq(getIntRandom(&random, 65, 80));
  ns[i].when = w * BASE_NOTE_DURATION;
  ns[i].duration = ceil(d / 2) * BASE_NOTE_DURATION;
  i++;
  w += d;
  d = getIntRandom(&random, 2, 6);
  ns[i].freq = midiNoteToFreq(getIntRandom(&random, 70, 85));
  ns[i].when = w * BASE_NOTE_DURATION;
  ns[i].duration = ceil(d / 2) * BASE_NOTE_DURATION;
  i++;
  ns[i].freq = -1;
}

void generateSoundEffect() {
  coin(soundEffects[COIN]);
  laser(soundEffects[LASER]);
  explosion(soundEffects[EXPLOSION]);
  powerUp(soundEffects[POWER_UP]);
  hit(soundEffects[HIT]);
  jump(soundEffects[JUMP]);
  select(soundEffects[SELECT]);
  randomSe(soundEffects[RANDOM]);
  click(soundEffects[CLICK]);
}

float soundEffectPlayedTimes[SOUND_EFFECT_TYPE_COUNT];

void initSound() {
  nextTime = md_getAudioTime() + 1;
  setSeedWithTime(&random);
  generateSoundEffect();
  for (int i = 0; i < SOUND_EFFECT_TYPE_COUNT; i++) {
    soundEffectPlayedTimes[i] = 0;
  }
}

void updateSound() {
  if (!isSoundEnabled) {
    return;
  }
  float ct = md_getAudioTime();
  if (ct > nextTime) {
    nextTime = ct;
  }
  if (nextTime < ct + 1.0f / FPS * 2) {
    nextTime += 60.0f / tempo;
    md_playTone(900, BASE_NOTE_DURATION, nextTime);
    md_playTone(700, BASE_NOTE_DURATION, nextTime + BASE_NOTE_DURATION);
  }
}

void playSoundEffect(int type) {
  if (!isSoundEnabled) {
    return;
  }
  float ct = md_getAudioTime();
  float qt = ceil(ct / QUANTIZED_DURATION) * QUANTIZED_DURATION;
  if (qt <= soundEffectPlayedTimes[type]) {
    return;
  }
  for (int i = 0; i < MAX_SOUND_EFFECT_NOTE_LENGTH; i++) {
    Note *ns = &soundEffects[type][i];
    if (ns->freq == -1) {
      break;
    }
    md_playTone(ns->freq, ns->duration, qt + ns->when);
  }
  soundEffectPlayedTimes[type] = qt;
}
