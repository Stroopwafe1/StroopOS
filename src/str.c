#include "str.h"


uint32_t StrLen(const char* str) {
	uint32_t len = 0;
	while (str[len])
		len++;
	return len;
}

bool StrEquals(const char* str, uint8_t* cmp, uint32_t size) {
  for (uint32_t i = 0; i < size; i++) {
	if (str[i] != cmp[i]) return false;
  }
  return StrLen(str) == size;
}

void MemSet(uint8_t* mem, uint32_t byte_count, uint8_t value) {
  for (uint32_t i = 0; i < byte_count; i++) {
	mem[i] = value;
  }
}

void MemCopy(uint8_t* dest, uint8_t* src, uint32_t byte_count) {
  for (uint32_t i = 0; i < byte_count; i++) {
	dest[i] = src[i];
  }
}

char string_buffer[256] = { 0 };
char* ToStringUI32(uint32_t value) {
  uint32_t divider = 10;
  uint32_t length = 0;

  while (value != 0) {
	string_buffer[254 - length++] = (value % divider) + '0';
	value /= divider;
  }
  string_buffer[255] = '\0'; // CStr null byte
  if (length == 0) {
	string_buffer[254 - length++] = '0'; // If value was 0 from the start, return '0'
  }
  return &string_buffer[255 - length];
}
