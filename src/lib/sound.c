#include "sound.h"

#include <math.h>
#include <stdbool.h>
#include <string.h>

#include "cglp.h"
#include "machineDependent.h"
#include "random.h"

static Random soundRandom;
static float _rnd(float low, float high) {
  return getRandom(&soundRandom, low, high);
}
static int _rndi(int low, int high) {
  return getIntRandom(&soundRandom, low, high);
}

#define BASE_NOTE_DURATION (60.0f / tempo / 32)
#define QUANTIZED_DURATION (60.0f / tempo / 2)

bool isSoundEnabled = true;
bool isPlayingBgm;

typedef struct {
  float freq;
  float duration;
  float when;
} Note;

#define MAX_SOUND_EFFECT_NOTE_LENGTH 32
static Note soundEffects[SOUND_EFFECT_TYPE_COUNT]
                        [MAX_SOUND_EFFECT_NOTE_LENGTH + 1];
static float soundEffectPlayedTimes[SOUND_EFFECT_TYPE_COUNT];

static float midiNoteToFreq(int midiNote) {
  return 440 * pow(2, (float)(midiNote - 69) / 12);
}

static void addNotes(Note *ns, int count, int when, int from, int to,
                     float amplitudeFrom, float amplitudeTo) {
  float mn = from;
  float mo = (float)(to - from) / (count - 1);
  float an = amplitudeFrom;
  float ao = (float)(amplitudeTo - amplitudeFrom) / (count - 1);
  for (int i = 0; i < count; i++) {
    Note *n = &ns[i];
    n->freq = midiNoteToFreq((int)(mn + _rnd(-an, an)));
    n->duration = BASE_NOTE_DURATION;
    n->when = when * BASE_NOTE_DURATION;
    mn += mo;
    an += ao;
    when++;
  }
}

static void coinSe(Note *ns) {
  int i = 0;
  int w = 0;
  int d = _rndi(4, 8);
  ns[i].freq = midiNoteToFreq(_rndi(70, 80));
  ns[i].when = w * BASE_NOTE_DURATION;
  ns[i].duration = d * BASE_NOTE_DURATION;
  i++;
  w += d;
  d = _rndi(7, 15);
  ns[i].freq = midiNoteToFreq(_rndi(85, 95));
  ns[i].when = w * BASE_NOTE_DURATION;
  ns[i].duration = d * BASE_NOTE_DURATION;
  i++;
  ns[i].freq = -1;
}

static void laserSe(Note *ns) {
  int i = 0;
  int w = 0;
  int d = _rndi(9, 19);
  addNotes(&ns[i], d, w, _rndi(75, 95), _rndi(45, 65), _rndi(5, 9), 0);
  i += d;
  ns[i].freq = -1;
}

static void explosionSe(Note *ns) {
  int i = 0;
  int w = 0;
  int d = _rndi(5, 12);
  addNotes(&ns[i], d, w, _rndi(70, 90), _rndi(50, 60), _rndi(5, 15),
           _rndi(15, 25));
  i++;
  w += d;
  d = _rndi(12, 20);
  addNotes(&ns[i], d, w, _rndi(50, 70), _rndi(30, 50), _rndi(15, 25),
           _rndi(5, 15));
  i += d;
  ns[i].freq = -1;
}

static void powerUpSe(Note *ns) {
  int i = 0;
  int w = 0;
  int d = _rndi(2, 5);
  addNotes(&ns[i], d, w, _rndi(75, 85), _rndi(65, 75), 0, 0);
  i += d;
  w += d;
  d = _rndi(6, 9);
  addNotes(&ns[i], d, w, _rndi(65, 75), _rndi(85, 95), 0, 0);
  i += d;
  w += d;
  d = _rndi(3, 6);
  addNotes(&ns[i], d, w, _rndi(85, 95), _rndi(75, 85), 0, 0);
  i += d;
  w += d;
  d = _rndi(6, 9);
  addNotes(&ns[i], d, w, _rndi(75, 85), _rndi(95, 105), 0, 0);

  i += d;
  ns[i].freq = -1;
}

static void hitSe(Note *ns) {
  int i = 0;
  int w = 0;
  int d = _rndi(5, 9);
  addNotes(&ns[i], d, w, _rndi(70, 80), _rndi(60, 70), _rndi(0, 10),
           _rndi(0, 10));
  i += d;
  ns[i].freq = -1;
}

static void jumpSe(Note *ns) {
  int i = 0;
  int w = 0;
  int d = _rndi(2, 5);
  ns[i].freq = midiNoteToFreq(_rndi(70, 80));
  ns[i].when = w * BASE_NOTE_DURATION;
  ns[i].duration = d * BASE_NOTE_DURATION;
  i++;
  w += d;
  d = _rndi(9, 19);
  addNotes(&ns[i], d, w, _rndi(55, 70), _rndi(80, 95), 0, 0);
  i += d;
  ns[i].freq = -1;
}

static void selectSe(Note *ns) {
  int i = 0;
  int w = 0;
  int d = _rndi(2, 4);
  int f = _rndi(60, 90);
  ns[i].freq = midiNoteToFreq(f);
  ns[i].when = w * BASE_NOTE_DURATION;
  ns[i].duration = ceilf(d * 0.7f) * BASE_NOTE_DURATION;
  i++;
  w += d;
  d = _rndi(4, 9);
  f += _rndi(-2, 5);
  ns[i].freq = midiNoteToFreq(f);
  ns[i].when = w * BASE_NOTE_DURATION;
  ns[i].duration = d * BASE_NOTE_DURATION;
  i++;
  ns[i].freq = -1;
}

static void randomSe(Note *ns) {
  int i = 0;
  int w = 0;
  int d = _rndi(3, 15);
  addNotes(&ns[i], d, w, _rndi(30, 99), _rndi(30, 99), _rndi(0, 20),
           _rndi(0, 20));
  i += d;
  w += d;
  d = _rndi(3, 15);
  addNotes(&ns[i], d, w, _rndi(30, 99), _rndi(30, 99), _rndi(0, 20),
           _rndi(0, 20));
  i += d;
  ns[i].freq = -1;
}

static void clickSe(Note *ns) {
  int i = 0;
  int w = 0;
  int d = _rndi(2, 6);
  ns[i].freq = midiNoteToFreq(_rndi(65, 80));
  ns[i].when = w * BASE_NOTE_DURATION;
  ns[i].duration = ceilf(d / 2) * BASE_NOTE_DURATION;
  i++;
  w += d;
  d = _rndi(2, 6);
  ns[i].freq = midiNoteToFreq(_rndi(70, 85));
  ns[i].when = w * BASE_NOTE_DURATION;
  ns[i].duration = ceilf(d / 2) * BASE_NOTE_DURATION;
  i++;
  ns[i].freq = -1;
}

static void generateSoundEffect() {
  coinSe(soundEffects[COIN]);
  laserSe(soundEffects[LASER]);
  explosionSe(soundEffects[EXPLOSION]);
  powerUpSe(soundEffects[POWER_UP]);
  hitSe(soundEffects[HIT]);
  jumpSe(soundEffects[JUMP]);
  selectSe(soundEffects[SELECT]);
  randomSe(soundEffects[RANDOM]);
  clickSe(soundEffects[CLICK]);
}

void playSoundEffect(int type) {
  if (!isSoundEnabled) {
    return;
  }
  float ct = md_getAudioTime();
  float qt = ceilf(ct / QUANTIZED_DURATION) * QUANTIZED_DURATION;
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

#define MAX_BGM_NOTE_LENGTH 64
static Note bgm[MAX_BGM_NOTE_LENGTH + 1];
static int bgmNoteLength = 32;
static float bgmDuration;
static int bgmIndex;
static float bgmTime;

typedef struct {
  int midiNote;
  bool isMinor;
} Chord;

static Chord chords[][4] = {{
                                {.midiNote = 0, .isMinor = false},
                                {.midiNote = 0, .isMinor = false},
                                {.midiNote = 4, .isMinor = true},
                                {.midiNote = 9, .isMinor = true},
                            },
                            {
                                {.midiNote = 5, .isMinor = false},
                                {.midiNote = 5, .isMinor = false},
                                {.midiNote = 2, .isMinor = true},
                                {.midiNote = 2, .isMinor = true},
                            },
                            {
                                {.midiNote = 7, .isMinor = false},
                                {.midiNote = 7, .isMinor = false},
                                {.midiNote = 11, .isMinor = true},
                                {.midiNote = 11, .isMinor = true},
                            }};

static int nextChordsIndex[][3] = {
    {0, 1, 2},
    {1, 2, 0},
    {2, 0, 0},
};

static int keys[7] = {0, 2, 3, 5, 7, 9, 10};
static int progression[2][4] = {{0, 4, 7, 11}, {0, 3, 7, 10}};

static void generateChordProgression(int midiNotes[][4], int len) {
  int key = keys[_rndi(0, 7)];
  int octave = 3;
  int chordChangeInterval = 4;
  Chord chord;
  int chordIndex;
  for (int i = 0; i < len; i++) {
    if (i % chordChangeInterval == 0) {
      if (i == 0) {
        chordIndex = _rndi(0, 2);
        chord = chords[chordIndex][_rndi(0, 4)];
      } else if (_rnd(0, 1) < 0.8f - ((i / chordChangeInterval) % 2 * 0.5f)) {
        chordIndex = nextChordsIndex[chordIndex][_rndi(0, 3)];
        chord = chords[chordIndex][_rndi(0, 4)];
      }
    }
    int pi = chord.isMinor ? 1 : 0;
    for (int j = 0; j < 4; j++) {
      midiNotes[i][j] = octave * 12 + 12 + chord.midiNote + progression[pi][j];
    }
  }
}

static void reversePattern(bool pattern[], int interval, int len, int freq) {
  bool pt[MAX_BGM_NOTE_LENGTH];
  for (int i = 0; i < interval; i++) {
    pt[i] = false;
  }
  for (int i = 0; i < freq; i++) {
    pt[_rndi(0, interval)] = true;
  }
  for (int i = 0; i < len; i++) {
    if (pt[i % interval]) {
      pattern[i] = !pattern[i];
    }
  }
}

static void createRandomPattern(bool pattern[], int len, int freq) {
  int interval = 4;
  for (int i = 0; i < len; i++) {
    pattern[i] = false;
  }
  while (interval < len) {
    reversePattern(pattern, interval, len, freq);
    interval *= 2;
  }
}

static void generateBgm() {
  int noteLength = bgmNoteLength;
  int chordMidiNotes[MAX_BGM_NOTE_LENGTH][4];
  generateChordProgression(chordMidiNotes, noteLength);
  bool pattern[MAX_BGM_NOTE_LENGTH];
  createRandomPattern(pattern, noteLength, 1);
  bool continuingPattern[MAX_BGM_NOTE_LENGTH];
  for (int i = 0; i < noteLength; i++) {
    continuingPattern[i] = _rnd(0, 1) < 0.8f;
  }
  float duration = bgmDuration;
  float when = -duration;
  bool hasPrevNote = false;
  int bgmNoteIndex = 0;
  for (int i = 0; i < noteLength; i++) {
    when += duration;
    if (!pattern[i]) {
      hasPrevNote = false;
      continue;
    }
    if (continuingPattern[i] && hasPrevNote) {
      bgm[bgmNoteIndex - 1].duration += duration;
      continue;
    }
    hasPrevNote = true;
    int mn = chordMidiNotes[i][_rndi(0, 4)];
    bgm[bgmNoteIndex].freq = midiNoteToFreq(mn);
    bgm[bgmNoteIndex].when = when;
    bgm[bgmNoteIndex].duration = duration;
    bgmNoteIndex++;
  }
  bgm[bgmNoteIndex].freq = -1;
}

void initSound(char *title, char *description, int soundSeed) {
  isPlayingBgm = false;
  char randomSeedStr[99];
  strncpy(randomSeedStr, title, 98);
  strncat(randomSeedStr, description, 98);
  int seed = getHashFromString(randomSeedStr) + soundSeed;
  setRandomSeed(&soundRandom, seed);
  bgmIndex = 0;
  bgmDuration = BASE_NOTE_DURATION * 8;
  generateSoundEffect();
  for (int i = 0; i < SOUND_EFFECT_TYPE_COUNT; i++) {
    soundEffectPlayedTimes[i] = 0;
  }
  generateBgm();
  float ct = md_getAudioTime();
  bgmTime = ceilf(ct / QUANTIZED_DURATION) * QUANTIZED_DURATION;
}

void updateSound() {
  if (!isSoundEnabled || !isPlayingBgm) {
    return;
  }
  float ct = md_getAudioTime();
  Note *bn = &bgm[bgmIndex];
  float bt = bgmTime + bn->when;
  if (ct > bt) {
    bgmIndex = 0;
    bgmTime = ceilf(ct / QUANTIZED_DURATION) * QUANTIZED_DURATION;
  } else if (bt < ct + 1.0f / FPS * 2) {
    md_playTone(bn->freq, bn->duration, bt);
    bgmIndex++;
    if (bgm[bgmIndex].freq == -1) {
      bgmIndex = 0;
      bgmTime += bgmNoteLength * bgmDuration;
    }
  }
}
