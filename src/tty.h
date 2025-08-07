#ifndef TTY_H
#define TTY_H

#include <stdint.h>
#include <stdbool.h>

#include "psf_render.h"
#include "structs.h"
#include "font.h"
#include "keyboard.h"

extern uint32_t term_x;
extern uint32_t term_y;

extern mb_framebuffer* mb_fb;

#define CHAR_WIDTH 6
#define CHAR_HEIGHT 8

#define Print_(text, colour, newline) DrawString((text), &term_x, &term_y, (colour), (newline))
#define PrintError(text, newline) Print_((text), (ARGB){0xFFFF0000}, (newline))
#define PrintOutput(text, newline) Print_((text), (ARGB){0xFF00FF00}, (newline))
#define PrintInput(text, newline) Print_((text), (ARGB){0xFFFFFFFF}, (newline))

void CaretMoveUp();
void CaretMoveDown();
void CaretMoveLeft();
void CaretMoveRight();

void DrawPixel(uint32_t x, uint32_t y, uint32_t colour);
void DrawChar(char c, uint32_t x, uint32_t y, uint32_t colour);
void DrawString(const char* str, uint32_t* x, uint32_t* y, ARGB colour, bool new_line);
void DrawCaret(ARGB colour);

void HandleKey(Key_Packet key, Reg_State* r);

#endif
