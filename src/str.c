#include "str.h"


uint32_t strlen(const char* str) {
	uint32_t len = 0;
	while (str[len])
		len++;
	return len;
}

bool strequals(const char* str, uint8_t* cmp, uint32_t size) {
  for (uint32_t i = 0; i < size; i++) {
	if (str[i] != cmp[i]) return false;
  }
  return strlen(str) == size;
}

void memset(uint8_t* mem, uint32_t byte_count, uint8_t value) {
  for (uint32_t i = 0; i < byte_count; i++) {
	mem[i] = value;
  }
}

void memmove(uint8_t* dest, uint8_t* src, uint32_t byte_count) {
  for (uint32_t i = 0; i < byte_count; i++) {
	dest[i] = src[i];
  }
}
