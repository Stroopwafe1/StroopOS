#include "kalloc_page.h"
#include "str.h"
#include "kernel_functions.h"

uint32_t mmap_ram_offset = 0;

PageDescriptorEntry* kalloc_page(uint32_t pagesize) {
  *kpt_entry_count += 1;
  uint32_t hole = *kpt_entry_count;

  if (mmap_ram_offset + pagesize > mb_mmap_ram->length) {
	kPanic("Not enough RAM to allocate page");
	return (void*)0;
  }
  
  PageDescriptorEntry entry = {
	.start = (void*)(mb_mmap_ram->base_address + mmap_ram_offset),
	.size = pagesize,
	.flags = KPAGE_IN_USE | KPAGE_READABLE | KPAGE_WRITABLE,
	.used_sections_bitmap = 0
  };

  mmap_ram_offset += pagesize;
  
  for (; entry.start < kpt_entries[hole / 2].start; hole /= 2) {
	MemCopy((void*)&kpt_entries[hole], (void*)&kpt_entries[hole / 2], sizeof(PageDescriptorEntry));
  }

  MemCopy((void*)&kpt_entries[hole], (void*)&entry, sizeof(PageDescriptorEntry));
  return &kpt_entries[hole];
}

uint32_t kalloc_page_get_free(PageDescriptorEntry* e) {
  uint32_t highest_set_bit_index = _asm_bsr(e->used_sections_bitmap);
  return e->size / 32 * (highest_set_bit_index - 1); // Return number of available sections in page
}

PageDescriptorEntry* kalloc_get_page(uint32_t index) {
  if (index >= *kpt_entry_count) {
	kPanic("[kalloc_get_page]: Index out of bounds");
	return (void*)0;
  }

  return &kpt_entries[index + 1]; // Index 0 is never used or accessible
}
