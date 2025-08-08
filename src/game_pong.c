#include "game_pong.h"
#include "kernel_functions.h"
#include "str.h"
#include "locale-qwerty-us.h"
#include "graphics.h"

#define BALL_WIDTH 10
#define BALL_HEIGHT 10
#define BALL_SPEED 20
#define PLAYER_WIDTH 100
#define PLAYER_SPEED 5
#define PLAYER_HEIGHT 10

int32_t ball_x = 10;
int32_t ball_y = 20;
int32_t ball_dx = 1;
int32_t ball_dy = 1;

int32_t player_x = 500;
int32_t player_y = 700;

bool game_paused = false;

uint32_t score = 0;

void DrawBall() {
  DrawRectangle(ball_x, ball_x + BALL_WIDTH, ball_y, ball_y + BALL_HEIGHT, 0xFFFFFFFF);
}

void DrawPlayer() {
  DrawRectangle(player_x, player_x + PLAYER_WIDTH, player_y, player_y + PLAYER_HEIGHT, 0xFF00FF00);
}

void Restart_PONG() {
  ball_x = 10;
  ball_y = 20;
  ball_dx = 1;
  ball_dy = 1;

  player_x = 500;
  player_y = 700;

  game_paused = false;
  score = 0;
}

void Update_PONG(void) {
  if (game_paused) {
	return;
  }
  Clear();

  uint32_t score_x = 0;
  uint32_t score_y = 0;
  DrawString("Score: ", &score_x, &score_y, COLOUR_GREEN, false);
  DrawString(ToStringUI32(score), &score_x, &score_y, COLOUR_WHITE, false);
  
  if (ball_x + ball_dx * BALL_SPEED > (int32_t)mb_fb->width) ball_dx *= -1;
  else if (ball_x + ball_dx * BALL_SPEED < 0) ball_dx *= -1;
  else if (ball_y + ball_dy * BALL_SPEED > (int32_t)mb_fb->height) ball_dy *= -1;
  else if ((int32_t)ball_y + ball_dy * BALL_SPEED < 0) ball_dy *= -1;

  if (ball_y + BALL_HEIGHT > player_y) {
	if (ball_x > player_x && ball_x < player_x + PLAYER_WIDTH) {
	  // We hit the ball
	  
	  ball_dy *= -1;
	  score++;
	  if (ball_x < player_x + PLAYER_WIDTH / 2) {
		// We hit on the left side
		ball_dx = -1;
	  } else {
		// We hit on the right side
		ball_dx = 1;
	  }
	} else {
	  // We didn't hit the ball, and lost
	  uint32_t text_x = 385;
	  uint32_t text_y = 10;
	  DrawString("You lost! Hit", &text_x, &text_y, COLOUR_WHITE, false);
	  DrawString(" R ", &text_x, &text_y, COLOUR_GREEN, false);
	  DrawString("to restart", &text_x, &text_y, COLOUR_WHITE, false);
	  game_paused = true;
	}
  }
  
  ball_x += ball_dx * BALL_SPEED;
  ball_y += ball_dy * BALL_SPEED;
  
  DrawBall();
  DrawPlayer();
}

void Init_PONG(void) {
  Clear();
}

void HandleKey_PONG(Key_Packet key, Reg_State* r)  {
  (void)r; // Unused
  if (key.scancode == KEY_ESCAPE) {
	kChangeState(TTY);
	return;
  } else if (key.scancode == KEY_R) {
	Restart_PONG();
  }
  
  if (key.state == PRESSED && !game_paused) {
	if (key.scancode == KEY_A) {
	  player_x = (player_x - PLAYER_SPEED) < 0 ? 0 : player_x - PLAYER_SPEED;
	} else if (key.scancode == KEY_D) {
	  player_x = (player_x + PLAYER_SPEED) > ((int32_t)mb_fb->width - PLAYER_WIDTH) ? player_x : player_x + PLAYER_SPEED;
	}
  }
}

void pong_register() {
  kSetHandler(GAME_PONG, &HandleKey_PONG);
  kSetInit(GAME_PONG, &Init_PONG);
  kSetUpdate(GAME_PONG, 3, &Update_PONG);
}
