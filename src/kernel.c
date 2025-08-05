#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "structs.h"
#include "psf_render.h"
#include "font.h"

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif


#define CHAR_WIDTH 6
#define CHAR_HEIGHT 8

mb_info* multiboot_info;
mb_framebuffer* mb_fb;

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

void DrawString(const char* str, uint32_t* x, uint32_t* y, ARGB colour) {
    while (*str) {
	  putchar(*str++, x, y, colour, (ARGB){ 0 });
	  //DrawChar(*str++, *x, *y, colour);
	  //*x += CHAR_WIDTH;
    }
}



size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

#define MB_FB_INFO_TYPE 8

uint32_t nearest_multiple(uint32_t to_round, uint32_t multiple) {
  return to_round + (multiple - (to_round % multiple)) % multiple;
}

mb_framebuffer* find_framebuffer() {
  uint32_t* tag_start = (uint32_t*)(void*)(multiboot_info+8);
  while (*tag_start) {
	uint32_t tag_size = *(tag_start + 1);
	if (*tag_start == MB_FB_INFO_TYPE) {
	  return (mb_framebuffer*)tag_start;
	} else {
	  tag_start += nearest_multiple(tag_size, 8) / sizeof(uint32_t);
	}
  }
  return NULL;
}

void kernel_main(void) {
  mb_fb = find_framebuffer();
  ARGB yellow = { 0x00FFFF00 };
  ARGB purple = { 0x00FF00FF };
  
  uint32_t x = 0;
  uint32_t y = 0;
  
  DrawString("Hello from kernel\n", &x, &y, yellow);
  DrawString("From another line as well", &x, &y, purple);
}
