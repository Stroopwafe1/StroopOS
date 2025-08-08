#include "tty.h"

#define FONT_H_DEFINITION
#include "font.h"

#define KEY_MAPPING
#include "locale-qwerty-us.h"
#include "str.h"
#include "kernel_functions.h"
#include "graphics.h"

uint32_t term_x = 0;
uint32_t term_y = 0;
bool caret_shown = true;

term_char screen[768][1024] = {0};

void Update_TTY(void) {
  	caret_shown = !caret_shown;
	ARGB colour = caret_shown ? COLOUR_WHITE : COLOUR_BLACK;
	DrawCaret(colour);
}

void Init_TTY(void) {
  Clear();
  term_x = 0;
  term_y = 0;
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

void DrawCaret(ARGB colour) {
  composechar('_', term_x, term_y, colour);
}

uint8_t buffer[256] = { 0 };
uint32_t buffer_size = 0;

void CleanBuffer() {
  MemSet(&buffer[0], buffer_size, 0);
  buffer_size = 0;
}

void Clear_TTY() {
  Clear();
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
	  putchar(entry.c, &cx, &cy, entry.colour, COLOUR_BLACK);
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
  if (StrEquals("/echo ", buffer, 6)) {
	PrintOutput(str_buf + 6, true);
  } else if (StrEquals("/clear", buffer, buffer_size)) {
	Clear_TTY();
  } else if (StrEquals("/help", buffer, buffer_size) || buffer[0] == '?') {
	PrintUsage();
  } else if (StrEquals("/pong", buffer, buffer_size)) {
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
  (void)r; // Unused
  DrawCaret(COLOUR_BLACK);
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
	  MemCopy((void*)&screen[i], (void*)&screen[i + 1], mb_fb->width);
	}
	Redraw();
	return;
  } else if (key.scancode == KEY_BACKSPACE) {
	term_x--;
	putchar(' ', &term_x, &term_y, COLOUR_BLACK, COLOUR_BLACK);
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

  if (mapping) {
	buffer[buffer_size++] = mapping;
	screen[term_y][term_x] = (term_char) { .colour = COLOUR_WHITE, .c = mapping };
	putchar(mapping, &term_x, &term_y, COLOUR_WHITE, COLOUR_BLACK);
  }

};

void tty_register() {
  kSetHandler(TTY, &HandleKey_TTY);
  kSetInit(TTY, &Init_TTY);
  kSetUpdate(TTY, 6, &Update_TTY);
}
