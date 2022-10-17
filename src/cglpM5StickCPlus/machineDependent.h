#ifndef MACHINE_DEPENDENT_H
#define MACHINE_DEPENDENT_H

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC extern
#endif

#define CHARACTER_WIDTH 6
#define CHARACTER_HEIGHT 6

EXTERNC void md_drawRect(float x, float y, float w, float h, unsigned char r,
                         unsigned char g, unsigned char b);
EXTERNC void md_drawCharacter(
    unsigned char grid[CHARACTER_HEIGHT][CHARACTER_WIDTH][3], float x, float y,
    int hash);
EXTERNC void md_clearView(unsigned char r, unsigned char g, unsigned char b);
EXTERNC void md_clearScreen(unsigned char r, unsigned char g, unsigned char b);
EXTERNC void md_playTone(float freq, float duration, float when);
EXTERNC void md_stopTone();
EXTERNC float md_getAudioTime();
EXTERNC void md_initView(int w, int h);
EXTERNC void md_consoleLog(char *msg);

#endif
