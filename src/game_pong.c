#include "game_pong.h"
#include "kernel_functions.h"
#include "str.h"
#include "locale-qwerty-us.h"

#define BALL_WIDTH 10
#define BALL_HEIGHT 10
#define BALL_SPEED 10

uint32_t ball_x = 10;
uint32_t ball_y = 20;
int32_t ball_dx = 1;
int32_t ball_dy = 1;

int32_t player_x = 500;
int32_t player_y = 700;


void DrawBall() {
  DrawRectangle(ball_x, ball_x + BALL_WIDTH, ball_y, ball_y + BALL_HEIGHT, 0xFFFFFFFF);
}

void Clear_PONG() {
  memset((void*)mb_fb->address, sizeof(uint32_t) * mb_fb->height * mb_fb->width, 0);
}

void Update_PONG(uint32_t delta_ticks) {
  Clear_PONG();

  if (ball_x + ball_dx * BALL_SPEED > mb_fb->width) ball_dx *= -1;
  else if ((int32_t)ball_x + ball_dx * BALL_SPEED < 0) ball_dx *= -1;
  else if (ball_y + ball_dy * BALL_SPEED > mb_fb->height) ball_dy *= -1;
  else if ((int32_t)ball_y + ball_dy * BALL_SPEED < 0) ball_dy *= -1;

  ball_x += ball_dx * BALL_SPEED;
  ball_y += ball_dy * BALL_SPEED;
  
  DrawBall();
  DrawRectangle(player_x, player_x + 100, player_y, player_y + 10, 0xFF00FF00);
}

void Init_PONG(void) {
  Clear_PONG();
}

void HandleKey_PONG(Key_Packet key, Reg_State* r)  {
  if (key.scancode == KEY_ESCAPE || key.scancode == KEY_Q) {
	kChangeState(TTY);
	return;
  }
  if (key.state == PRESSED) {
	if (key.scancode == KEY_A) {
	  player_x = (player_x - 5) < 0 ? 0 : player_x - 5;
	} else if (key.scancode == KEY_D) {
	  player_x = (player_x + 5) > (mb_fb->width - 100) ? player_x : player_x + 5;
	}
  }
}

void pong_register() {
  kSetHandler(GAME_PONG, &HandleKey_PONG);
  kSetInit(GAME_PONG, &Init_PONG);
  kSetUpdate(GAME_PONG, 3, &Update_PONG);
}

void DrawPixel_PONG(uint32_t x, uint32_t y, uint32_t colour) {
  uint32_t* fb_addr = (uint32_t*)(mb_fb->address);
  
  uint32_t* pixel = fb_addr + mb_fb->pitch / 4 * y + x;
  *pixel = colour;
}

void DrawRectangle(uint32_t x1, uint32_t x2, uint32_t y1, uint32_t y2, uint32_t colour) {
  for (uint32_t y = y1; y < y2; y++) {
	for (uint32_t x = x1; x < x2; x++) {
	  DrawPixel_PONG(x, y, colour);
	}
  }
}
