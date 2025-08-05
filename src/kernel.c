#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "font.h"

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

typedef struct Multiboot_Information {
  uint32_t total_size;
  uint32_t reserved;
} mb_info;

typedef struct Colour {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} colour;

typedef struct Multiboot_Framebuffer_ColourPalette {
  uint16_t amount;
  colour palette[0];
} mb_fb_colour_palette;

typedef struct Multiboot_Framebuffer_DirectRGB {
  uint8_t red_pos;
  uint8_t red_mask;
  uint8_t green_pos;
  uint8_t green_mask;
  uint8_t blue_pos;
  uint8_t blue_mask;
} mb_fb_dRGB;

typedef struct Multiboot_Framebuffer {
  uint32_t tag_type;
  uint32_t size;
  uint64_t address;
  uint32_t pitch;
  uint32_t width;
  uint32_t height;
  uint8_t bits_per_pixel;
  uint8_t type;
  uint8_t reserved;
  union {
	mb_fb_colour_palette palette;
	mb_fb_dRGB dRGB;
  };
} mb_framebuffer;


#define CHAR_WIDTH 6
#define CHAR_HEIGHT 8

mb_info* multiboot_info;
mb_framebuffer* mb_fb;

void DrawPixel(uint32_t x, uint32_t y, uint32_t colour) {
  uint32_t* fb_addr = (uint32_t*)mb_fb->address;
  
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
			  DrawPixel(x+j, y+i, colour);
            }
        }
    }
}

void DrawString(const char* str, uint32_t x, uint32_t y, uint32_t colour) {
    while (*str) {
	  DrawChar(*str++, x, y, colour);
        x += CHAR_WIDTH;
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
  uint32_t* tag_start = multiboot_info+8;
  while (*tag_start) {
	uint32_t tag_size = *(tag_start + 1);
	if (*tag_start == MB_FB_INFO_TYPE) {
	  return (mb_framebuffer*)tag_start;
	} else {
	  tag_start += nearest_multiple(tag_size, 8) / sizeof(uint32_t);
	}
  }
}

void kernel_main(void) {
  mb_fb = find_framebuffer();
  for (uint32_t y = 0; y < mb_fb->height; y++) {
	for (uint32_t x = 0; x < mb_fb->width; x++) {
	  //DrawPixel(x, y);
	}
  }
  uint32_t colour = ((1 << mb_fb->dRGB.green_mask) - 1) << mb_fb->dRGB.green_pos;
  colour |= ((1 << mb_fb->dRGB.red_mask) - 1) << mb_fb->dRGB.red_pos;
  
  DrawString("Hello from kernel", 0, 0, colour);
}
