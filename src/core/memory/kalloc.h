#ifndef KALLOC_H
#define KALLOC_H

#include "structs.h"

extern MB_MMap* mb_mmap;
extern MB_MMapEntry* mb_mmap_ram;

void kalloc_init();
void* kalloc(uint32_t bytes);
void kfree(void* address, uint32_t bytes);

#endif
