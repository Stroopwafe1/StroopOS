#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "structs.h"
#include <stdbool.h>

extern mb_framebuffer* mb_fb;

typedef struct {
  ARGB colour;
  unsigned char c;
} term_char;

#define COLOUR_BLACK ((ARGB) {0x0})
#define COLOUR_RED ((ARGB) {0xFFFF0000})
#define COLOUR_GREEN ((ARGB) {0xFF00FF00})
#define COLOUR_WHITE ((ARGB) {0xFFFFFFFF})
#define COLOUR_ORANGE ((ARGB) {0xFF7FFF00})

#define Print_(text, colour, newline) DrawString((text), &term_x, &term_y, (colour), (newline))
#define PrintError(text, newline) Print_((text), COLOUR_RED, (newline))
#define PrintOutput(text, newline) Print_((text), COLOUR_GREEN, (newline))
#define PrintInput(text, newline) Print_((text), COLOUR_WHITE, (newline))

void DrawPixel(uint32_t x, uint32_t y, uint32_t colour);
void DrawChar(char c, uint32_t x, uint32_t y, uint32_t colour);
void DrawString(const char* str, uint32_t* x, uint32_t* y, ARGB colour, bool new_line);
void DrawRectangle(uint32_t x1, uint32_t x2, uint32_t y1, uint32_t y2, uint32_t colour);
void Clear();

#endif
