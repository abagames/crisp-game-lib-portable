#ifndef RANDOM_H
#define RANDOM_H

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC extern
#endif

#include <stdint.h>

typedef struct {
  uint32_t x;
  uint32_t y;
  uint32_t z;
  uint32_t w;
} Random;

EXTERNC uint32_t nextRandom(Random *random);
EXTERNC float getRandom(Random *random, float low, float high);
EXTERNC int getIntRandom(Random *random, int low, int high);
EXTERNC int getPlusOrMinusRandom(Random *random);
EXTERNC void setRandomSeed(Random *random, uint32_t w);
EXTERNC void setRandomSeedWithTime(Random *random);

#endif