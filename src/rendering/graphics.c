#include "graphics.h"
#include "psf_render.h"
#include "str.h"

extern term_char screen[768][1024];

void DrawPixel(uint32_t x, uint32_t y, uint32_t colour) {
  uint32_t* fb_addr = (uint32_t*)(mb_fb->address);
  
  uint32_t* pixel = fb_addr + mb_fb->pitch / 4 * y + x;
  *pixel = colour;
}

void DrawChar(char c, uint32_t x, uint32_t y, uint32_t colour) {
  putchar(c, &x, &y, (ARGB){colour}, COLOUR_BLACK);
}

void DrawString(const char* str, uint32_t* x, uint32_t* y, ARGB colour, bool new_line) {
    while (*str) {
	  screen[*y][*x] = (term_char) { .colour = colour, .c = *str };
	  putchar(*str++, x, y, colour, COLOUR_BLACK);
    }
	if (new_line)
	  putchar('\n', x, y, colour, COLOUR_BLACK);
}

void DrawRectangle(uint32_t x1, uint32_t x2, uint32_t y1, uint32_t y2, uint32_t colour) {
  for (uint32_t y = y1; y < y2; y++) {
	for (uint32_t x = x1; x < x2; x++) {
	  DrawPixel(x, y, colour);
	}
  }
}

void Clear() {
  MemSet((void*)mb_fb->address, sizeof(uint32_t) * mb_fb->height * mb_fb->width, 0);
}
