#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "structs.h"

extern char kdbus[128];

void keyboard_install();
void keyboard_handler(Reg_State* r);

#endif
