#ifndef MACHINE_DEPENDENT_H
#define MACHINE_DEPENDENT_H

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC extern
#endif

#include "cglp.h"

EXTERNC void md_rect(float x, float y, float w, float h);
EXTERNC void md_text(char l, float x, float y);
EXTERNC void md_character(char l, float x, float y);
EXTERNC void md_color(float r, float g, float b);
EXTERNC void md_clearView(float r, float g, float b);
EXTERNC void md_playTone(float freq, float duration, float when);
EXTERNC void md_stopTone();
EXTERNC float md_getAudioTime();

EXTERNC void md_setTexts(char grid[][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1],
                         int count);
EXTERNC void md_setCharacters(
    char grid[][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1], int count);

EXTERNC void md_consoleLog(char *msg);

#endif
