#ifndef KPAGE_ALLOC_H
#define KPAGE_ALLOC_H

#include "structs.h"

extern MB_MMap* mb_mmap;
extern MB_MMapEntry* mb_mmap_ram;

#define KPAGE_IN_USE (1 << 0)
#define KPAGE_READABLE (1 << 1)
#define KPAGE_WRITABLE (1 << 2)
#define KPAGE_EXECUTABLE (1 << 3)
#define KPAGE_SHARED (1 << 4)
#define KPAGE_RESERVED (1 << 5)

PageDescriptorEntry* kalloc_page(uint32_t pagesize);
uint32_t kalloc_page_get_free(PageDescriptorEntry* e);

#endif
