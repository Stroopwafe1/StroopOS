#include "kalloc.h"
#include "kalloc_page.h"

uint32_t nearest_power2(uint32_t base) {
  return (base << 1) & base;
}

void kalloc_init() {
  //*kpt_entry_count = 0;
}

void* kalloc(uint32_t bytes) {
  return (void*)0;
#if 0
  if (*kpt_entry_count == 0) {
	uint32_t page_size = nearest_power2(bytes);
	if (page_size < 4096) {
	  page_size = 4096;
	}
	PageDescriptorEntry* entry = kalloc_page(page_size);
	if (entry == 0) return entry;
	// 4096 / 32 => 128
	// bytes = 300
	// nearest_multiple_of(bytes, 128) => 256+128 => 384
	// 3 bits

	// bytes % 128, if != 0 => +1
	uint32_t chunk_size = entry->size / 32;
	uint32_t rest = bytes % chunk_size;
	uint32_t num_bits_to_flip = bytes / chunk_size + (rest == 0 ? 0 : 1);
	entry->used_sections_bitmap |= (pow(2, num_bits_to_flip) - 1) << (32 - num_bits_to_flip);
	return entry->start;
  } else {
	// This isn't the first allocation
	
  }
#endif
}

void kfree(void* address, uint32_t bytes) {
  
}
