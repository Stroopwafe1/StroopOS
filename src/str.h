#ifndef STR_H
#define STR_H

#include <stdint.h>
#include <stdbool.h>

uint32_t StrLen(const char* str);
bool StrEquals(const char* str, uint8_t* cmp, uint32_t size);
void MemSet(uint8_t* mem, uint32_t byte_count, uint8_t value);
void MemCopy(uint8_t* dest, uint8_t* src, uint32_t byte_count);
char* ToStringUI32(uint32_t value);

#endif
