#include "psf_render.h"

void putchar(uint32_t c, uint32_t* cursor_x, uint32_t* cursor_y, ARGB fg, ARGB bg) {
  void* font_start = &_binary_Lat2_Terminus16_psfu_start;
  PSF_font* font = (PSF_font*)font_start;

  if (c == '\n') {
	*cursor_y += 1;
	*cursor_x = 0;
	return;
  }

  int bpl = (font->width + 7) / 8;

  uint32_t index = c > 0 && c < font->numglyph ? c : 0;
  uint32_t header_size = font->headersize;
  uint32_t bpg = font->bytesperglyph;

  uint8_t* glyph = ((uint8_t*)font_start) + header_size + index * bpg;

  uint32_t offset =
	(*cursor_y * font->height * mb_fb->pitch) +
	(*cursor_x * (font->width + 1) * sizeof(PIXEL));

  for (uint32_t y = 0; y < font->height; y++) {
	uint32_t line = offset;
	uint32_t mask = 1 << (font->width - 1);

	for (uint32_t x = 0; x < font->width; x++) {
	  PIXEL* pixel = (PIXEL*)(mb_fb->address + line);
	  ARGB colour = *((uint32_t*)glyph) & mask ? fg : bg;
	  *pixel = colour.as_uint;
	  mask >>= 1;
	  line += sizeof(PIXEL);
	}

	glyph += bpl;
	offset += mb_fb->pitch;
  }
  *cursor_x += 1;
}

void composechar(uint32_t c, uint32_t cursor_x, uint32_t cursor_y, ARGB fg) {
  void* font_start = &_binary_Lat2_Terminus16_psfu_start;
  PSF_font* font = (PSF_font*)font_start;

  int bpl = (font->width + 7) / 8;

  uint32_t index = c > 0 && c < font->numglyph ? c : 0;
  uint32_t header_size = font->headersize;
  uint32_t bpg = font->bytesperglyph;

  uint8_t* glyph = ((uint8_t*)font_start) + header_size + index * bpg;

  uint32_t offset =
	(cursor_y * font->height * mb_fb->pitch) +
	(cursor_x * (font->width + 1) * sizeof(PIXEL));

  for (uint32_t y = 0; y < font->height; y++) {
	uint32_t line = offset;
	uint32_t mask = 1 << (font->width - 1);

	for (uint32_t x = 0; x < font->width; x++) {
	  PIXEL* pixel = (PIXEL*)(mb_fb->address + line);
	  if (*((uint32_t*)glyph) & mask) {
		*pixel = fg.as_uint;
	  }
	  mask >>= 1;
	  line += sizeof(PIXEL);
	}

	glyph += bpl;
	offset += mb_fb->pitch;
  }
}
