#ifndef GAME_PONG_H
#define GAME_PONG_H

#include "keyboard.h"

extern mb_framebuffer* mb_fb;

void pong_register();
void DrawPixel_PONG(uint32_t x, uint32_t y, uint32_t colour);
void DrawRectangle(uint32_t x1, uint32_t x2, uint32_t y1, uint32_t y2, uint32_t colour);

#endif
