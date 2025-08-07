#ifndef STR_H
#define STR_H

#include <stdint.h>
#include <stdbool.h>

uint32_t strlen(const char* str);
bool strequals(const char* str, uint8_t* cmp, uint32_t size);
void memset(uint8_t* mem, uint32_t byte_count, uint8_t value);
void memmove(uint8_t* dest, uint8_t* src, uint32_t byte_count);

#endif
