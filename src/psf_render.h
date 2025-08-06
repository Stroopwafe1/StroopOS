#ifndef PSF_RENDER_H
#define PSF_RENDER_H

#include "structs.h"
extern mb_framebuffer* mb_fb;
// Not actually a pointer, its location is the start of the font
extern void* _binary_Lat2_Terminus16_psfu_start;

#define PIXEL uint32_t

void putchar(uint32_t c, uint32_t* cursor_x, uint32_t* cursor_y, ARGB fg, ARGB bg);
void composechar(uint32_t c, uint32_t cursor_x, uint32_t cursor_y, ARGB fg);

#endif
