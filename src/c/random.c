#include "random.h"

#include <stdint.h>
#include <time.h>

uint32_t nextRandom(Random *random) {
  uint32_t t = random->x ^ (random->x << 11);
  random->x = random->y;
  random->y = random->z;
  random->z = random->w;
  random->w = random->w ^ (random->w >> 19) ^ (t ^ (t >> 8));
  return random->w;
}

float getRandom(Random *random, float low, float high) {
  return (float)(nextRandom(random) / 0xfffffffful) * (high - low) + low;
}

int getIntRandom(Random *random, int low, int high) {
  if (low == high) {
    return low;
  }
  return nextRandom(random) % (high - low) + low;
}

int getPlusOrMinusRandom(Random *random) {
  return getIntRandom(random, 0, 2) * 2 - 1;
}

void setSeed(Random *random, uint32_t w) {
  int loopCount = 32;
  random->w = 123456789ul;
  random->y = 362436069ul;
  random->z = 521288629ul;
  random->w = w;
  for (int i = 0; i < loopCount; i++) {
    nextRandom(random);
  }
}
void setSeedWithTime(Random *random) { setSeed(random, (unsigned)time(NULL)); }
