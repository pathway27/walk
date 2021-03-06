#include <vitasdk.h>

#include "draw.h"
#include "debug_font.h"

#define OSD_MAX_STRING_LENGTH  512

static SceDisplayFrameBuf g_framebuf;
static uint8_t g_font_scale = 1;

static RGBA g_color_text = {.rgba = {255, 255, 255, 255}};
static RGBA g_color_bg   = {.rgba = {  0,   0,   0, 255}};

RGBA osdBlendColor(RGBA fg, RGBA bg) {
    uint8_t inv_alpha = 255 - fg.rgba.a;

    RGBA result;
    result.rgba.b = ((fg.rgba.a * fg.rgba.b + inv_alpha * bg.rgba.b) >> 8); // B
    result.rgba.g = ((fg.rgba.a * fg.rgba.g + inv_alpha * bg.rgba.g) >> 8); // G
    result.rgba.r = ((fg.rgba.a * fg.rgba.r + inv_alpha * bg.rgba.r) >> 8); // R
    result.rgba.a = 0xFF;                                         // A
    return result;
}

void osdUpdateFrameBuf(const SceDisplayFrameBuf *pParam) {
    memcpy(&g_framebuf, pParam, sizeof(SceDisplayFrameBuf));
}

void osdSetTextColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    g_color_text.rgba.r = r;
    g_color_text.rgba.g = g;
    g_color_text.rgba.b = b;
    g_color_text.rgba.a = a;
}
void osdSetBgColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    g_color_bg.rgba.r = r;
    g_color_bg.rgba.g = g;
    g_color_bg.rgba.b = b;
    g_color_bg.rgba.a = a;
}

void osdSetTextScale(uint8_t scale) {
    g_font_scale = scale;
}

/**
 * Draws alpha-blended rectangle at 1/4 res.
 */
void osdFastDrawRectangle(int x, int y, int width, int height) {
    if (g_color_bg.rgba.a == 0 || g_color_bg.rgba.a == 255) {
        osdDrawRectangle(x, y, width, height);
    }

    for (int yy = y; yy < y + height; yy += 2) {
        for (int xx = x; xx < x + width; xx += 2) {
            RGBA *pixelRGB = (RGBA *)g_framebuf.base + yy * g_framebuf.pitch + xx;
            RGBA newColor = osdBlendColor(g_color_bg, *pixelRGB);
            *pixelRGB = newColor;
            *(pixelRGB + 1) = newColor;
            *(pixelRGB + g_framebuf.pitch) = newColor;
            *(pixelRGB + g_framebuf.pitch + 1) = newColor;
        }
    }
}

/**
 * Draws alpha-blended rectangle
 */
void osdDrawRectangle(int x, int y, int width, int height) {
    if (g_color_bg.rgba.a == 0)
        return;

    if (g_color_bg.rgba.a == 255) {
        for (int yy = y; yy < y + height; yy++) {
            memset((uint32_t *)g_framebuf.base + (yy + y) * g_framebuf.pitch + x,
                g_color_bg.uint32,
                sizeof(uint32_t) * width);
        }
        return;
    }

    for (int yy = y; yy < y + height; yy++) {
        for (int xx = x; xx < x + width; xx++) {
            RGBA *pixelRGB = (RGBA *)g_framebuf.base + yy * g_framebuf.pitch + xx;
            *pixelRGB = osdBlendColor(g_color_bg, *pixelRGB);
        }
    }
}

/**
 * Draws single character
 */
void osdDrawCharacter(const char character, int x, int y) {
    if (character < 0 || character > 127)
        return;

    for (int yy = 0; yy < FONT_HEIGHT * g_font_scale; yy++) {
        uint32_t displacement = x + (y + yy) * g_framebuf.pitch;

        if (displacement >= g_framebuf.pitch * g_framebuf.height)
            return; // out of bounds

        int yy_font = yy / g_font_scale;
        RGBA *screenRGB = (RGBA *)g_framebuf.base + displacement;

        for (int xx = 0; xx < FONT_WIDTH * g_font_scale; xx++) {
            if (x + xx >= g_framebuf.width)
                return; // out of bounds

            // Get px 0/1 from osd_font.h
            int xx_font = xx / g_font_scale;
            char * bitmap = debug_font[character];
            int set = bitmap[yy_font] & 1 << xx_font;
            RGBA clr = set ? g_color_text : g_color_bg;

            if (clr.rgba.a) { // alpha != 0
                if (clr.rgba.a != 0xFF) { // alpha < 255
                    *(screenRGB + xx) = osdBlendColor(clr, *(screenRGB + xx)); // blend FG/BG color
                } else {
                    *(screenRGB + xx) = clr;
                }
            }
        }
    }
}

/**
 * Draws string
 */
void osdDrawString(int x, int y, const char *str) {
    for (size_t i = 0; i < strlen(str); i++)
        osdDrawCharacter(str[i], x + i * FONT_WIDTH * g_font_scale, y);
}

/**
 * Draws formatted string
 */
void osdDrawStringF(int x, int y, const char *format, ...) {
    char buffer[OSD_MAX_STRING_LENGTH] = "";
    va_list va;

    va_start(va, format);
    vsnprintf(buffer, OSD_MAX_STRING_LENGTH, format, va);
    va_end(va);

    osdDrawString(x, y, buffer);
}