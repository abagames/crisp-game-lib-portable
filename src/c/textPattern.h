#ifndef TEXT_PATTERN_H
#define TEXT_PATTERN_H

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC extern
#endif

#include "cglp.h"

EXTERNC char textPatterns[TEXT_PATTERN_COUNT][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1];

#endif
