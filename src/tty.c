#include "tty.h"

#define FONT_H_DEFINITION
#include "font.h"

uint32_t term_x = 0;
uint32_t term_y = 0;

void DrawPixel(uint32_t x, uint32_t y, uint32_t colour) {
  uint32_t* fb_addr = (uint32_t*)(mb_fb->address);
  
  uint32_t* pixel = fb_addr + mb_fb->pitch / 4 * y + x;
  *pixel = colour;
};

void DrawChar(char c, uint32_t x, uint32_t y, uint32_t colour) {
    if (c < ' ')
        c = 0;
    else 
        c -= ' ';
	
    const unsigned char* chr = font[c];

    // Draw pixels
    for (uint32_t j = 0; j < CHAR_WIDTH; j++) {
        for (uint32_t i = 0; i < CHAR_HEIGHT; i++) {
            if (chr[j] & (1<<i)) {
			  DrawPixel(x + j, y + i, colour);
            }
        }
    }
}

void DrawString(const char* str, uint32_t* x, uint32_t* y, ARGB colour, bool new_line) {
    while (*str) {
	  putchar(*str++, x, y, colour, (ARGB){ 0 });
	  //DrawChar(*str++, *x, *y, colour);
	  //*x += CHAR_WIDTH;
    }
	if (new_line)
	  putchar('\n', x, y, colour, (ARGB){0});
}
