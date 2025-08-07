#include "tty.h"

#define FONT_H_DEFINITION
#include "font.h"

#define KEY_MAPPING
#include "locale-qwerty-us.h"
#include "str.h"
#include "kernel_functions.h"

uint32_t term_x = 0;
uint32_t term_y = 0;
bool caret_shown = true;

typedef struct {
  ARGB colour;
  unsigned char c;
} term_char;

term_char screen[768][1024] = {0};

void Update_TTY(uint32_t delta) {
  	caret_shown = !caret_shown;
	ARGB colour = caret_shown ? (ARGB){0xFFFFFFFF} : (ARGB){0x0};
	DrawCaret(colour);
}

void Init_TTY(void) {
  Clear_TTY();
  PrintUsage();
}

void CaretMoveUp() {
  if (term_y == 0) return;
  term_y--;
}
void CaretMoveDown() {
  if (term_y == mb_fb->height) return;
  term_y++;
}
void CaretMoveLeft() {
  if (term_x == 0) { term_x = mb_fb->width - 1; return; }
  term_x--;
}
void CaretMoveRight() {
  term_x = (term_x + 1) % mb_fb->width;
}

void DrawPixel_TTY(uint32_t x, uint32_t y, uint32_t colour) {
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
			  DrawPixel_TTY(x + j, y + i, colour);
            }
        }
    }
}

void DrawString(const char* str, uint32_t* x, uint32_t* y, ARGB colour, bool new_line) {
    while (*str) {
	  screen[*y][*x] = (term_char) { .colour = colour, .c = *str };
	  putchar(*str++, x, y, colour, (ARGB){ 0 });
	  //DrawChar(*str++, *x, *y, colour);
	  //*x += CHAR_WIDTH;
    }
	if (new_line)
	  putchar('\n', x, y, colour, (ARGB){0});
}

void DrawCaret(ARGB colour) {
  composechar('_', term_x, term_y, colour);
}

uint8_t buffer[256] = { 0 };
uint32_t buffer_size = 0;

void CleanBuffer() {
  memset(&buffer[0], buffer_size, 0);
  buffer_size = 0;
}

void Clear_TTY() {
    memset((void*)mb_fb->address, sizeof(uint32_t) * mb_fb->height * mb_fb->width, 0);
	CleanBuffer();
	term_x = 0;
	term_y = 0;
}

void Redraw() {
  Clear_TTY();
  for (uint32_t y = 0; y < mb_fb->height; y++) {
	for (uint32_t x = 0; x < mb_fb->width; x++) {
	  uint32_t cy = y;
	  uint32_t cx = x;
	  term_char entry = screen[y][x];
	  if (entry.c == 0) continue;
	  putchar(entry.c, &cx, &cy, entry.colour, (ARGB){0});
	}
  }
}

void PrintUsage() {
  PrintOutput("StroopOS - Help\n"
			 "/echo <text> - Replies with the text supplied\n"
			 "/clear - Clears the screen\n"
			 "/pong - Play the classic arcade game Pong! (Press Escape to go back)\n"
			 "/help - Prints this usage message", true);
}

void ProcessBuffer() {
  char* str_buf = (char*)(void*)&buffer[0];
  if (strequals("/echo ", buffer, 6)) {
	PrintOutput(str_buf + 6, true);
  } else if (strequals("/clear", buffer, buffer_size)) {
	Clear_TTY();
  } else if (strequals("/help", buffer, buffer_size) || buffer[0] == '?') {
	PrintUsage();
  } else if (strequals("/pong", buffer, buffer_size)) {
	kChangeState(GAME_PONG);
  } else {
	if (buffer[0] == '/') {
	  PrintError("[Kernel TTY]: Unrecognised command ", false);
	  PrintInput(str_buf, false);
	  PrintError(" use ", false);
	  PrintOutput("/help", false);
	  PrintError(" to see available commands", true);
	}
  }
  CleanBuffer();
}

void HandleKey_TTY(Key_Packet key, Reg_State* r) {
  DrawCaret((ARGB){0x0});
  if (key.state == PRESSED) {
	switch(key.scancode & 0xFF) {
	case 0x48: CaretMoveUp(); return;
	case 0x4B: CaretMoveLeft(); return;
	case 0x4D: CaretMoveRight(); return;
	case 0x50: CaretMoveDown(); return;
	default: break;
	}
  }
  
  if (key.state != RELEASED) return;

  if (key.scancode == KEY_ENTER) {
	term_x = 0;
	term_y++;
	ProcessBuffer();
	return;
  } else if (key.scancode == KEY_KP_3) { 
	for (uint32_t i = 0; i < mb_fb->height - 1; i++) {
	  memmove((void*)&screen[i], (void*)&screen[i + 1], mb_fb->width);
	}
	Redraw();
	return;
  } else if (key.scancode == KEY_BACKSPACE) {
	term_x--;
	putchar(' ', &term_x, &term_y, (ARGB){0}, (ARGB){0x0});
	term_x--;
	if (buffer_size != 0)
	  buffer[--buffer_size] = 0;
  }

  unsigned char mapping = mapL1[key.scancode];
  if ((key.flags & MOD_SHIFT_LEFT) |
	  (key.flags & MOD_SHIFT_RIGHT) |
	  (key.flags & TOGGLE_CAPS)) {
	mapping = mapL2[key.scancode];
  }

  
	//case 0x0E: /* backspace */ 
  if (mapping) {
	buffer[buffer_size++] = mapping;
	screen[term_y][term_x] = (term_char) { .colour = (ARGB){0xFFFFFFFF}, .c = mapping };
	putchar(mapping, &term_x, &term_y, (ARGB){0xFFFFFFFF}, (ARGB){0x0});
  }

};

void tty_register() {
  kSetHandler(TTY, &HandleKey_TTY);
  kSetInit(TTY, &Init_TTY);
  kSetUpdate(TTY, 6, &Update_TTY);
}
