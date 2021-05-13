#ifndef _DRAW_H_
#define _DRAW_H_

#include <psp2/display.h>

typedef union {
    struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    } rgba;
    uint32_t uint32;
} RGBA;

int vsnprintf(char *s, size_t n, const char *format, va_list arg);

void osdUpdateFrameBuf(const SceDisplayFrameBuf *param);
void osdSetBgColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void osdSetTextColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void osdSetTextScale(uint8_t scale);

void osdDrawRectangle(int x, int y, int width, int height);
void osdFastDrawRectangle(int x, int y, int width, int height);

void osdDrawCharacter(const char character, int x, int y);
void osdDrawString(int x, int y, const char *str);
void osdDrawStringF(int x, int y, const char *format, ...);

#endif