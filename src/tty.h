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

void tty_register();

void CaretMoveUp();
void CaretMoveDown();
void CaretMoveLeft();
void CaretMoveRight();

void DrawCaret(ARGB colour);

void CleanBuffer();
void Redraw_TTY();
void PrintUsage();
void ProcessBuffer();

#endif
