#include "kalloc.h"
#include "kalloc_page.h"

uint32_t nearest_power2(uint32_t base) {
  return (base << 1) & base;
}

void kalloc_init() {
  *kpt_entry_count = 0;
}

void* kalloc(uint32_t bytes) {
  if (*kpt_entry_count == 0) {
	uint32_t page_size = nearest_power2(bytes);
	if (page_size < 4096) {
	  page_size = 4096;
	}
	PageDescriptorEntry* entry = kalloc_page(page_size);
	uint32_t num_bits_to_flip = bytes / (entry->size / 32);
	// TODO: Finish this
  }
}

void kfree(void* address, uint32_t bytes) {
  
}
