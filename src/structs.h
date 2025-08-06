#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdint.h>

typedef struct Multiboot_Information {
  uint32_t total_size;
  uint32_t reserved;
} mb_info;

typedef struct MB_Colour {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} MB_colour;

typedef struct Multiboot_Framebuffer_ColourPalette {
  uint16_t amount;
  MB_colour palette[0];
} mb_fb_colour_palette;

typedef union ARGB {
  uint32_t as_uint;
  struct {
	uint8_t alpha;
	uint8_t red;
	uint8_t green;
	uint8_t blue;
  } components;
} ARGB;

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

#define PSF1_FONT_MAGIC 0x0436

typedef struct {
    uint16_t magic; // Magic bytes for identification.
    uint8_t fontMode; // PSF font mode.
    uint8_t characterSize; // PSF character size.
} PSF1_Header;


#define PSF_FONT_MAGIC 0x864ab572

typedef struct {
    uint32_t magic;         /* magic bytes to identify PSF */
    uint32_t version;       /* zero */
    uint32_t headersize;    /* offset of bitmaps in file, 32 */
    uint32_t flags;         /* 0 if there's no unicode table */
    uint32_t numglyph;      /* number of glyphs */
    uint32_t bytesperglyph; /* size of each glyph */
    uint32_t height;        /* height in pixels */
    uint32_t width;         /* width in pixels */
} PSF_font;

typedef struct {
  uint32_t GS, FS, ES, DS;
  uint32_t EDI, ESI, EBP, ESP, EBX, EDX, ECX, EAX;
  uint32_t intr_index, err_code;
  uint32_t EIP, CS, EFLAGS, USERESP, SS;
} Reg_State;

#endif
