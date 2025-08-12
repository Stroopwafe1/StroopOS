#include "kalloc_page.h"
#include "str.h"
#include "kernel_functions.h"

/**
   Prioritisation
   Things to consider:
   1. How do I want it to be prioritised? (Access Count, Memory Address, Free/Used sections?)
   2. Should it even be a PQueue, or some other data structure?
   3. Why do Linux and Windows use an AVL tree, and how do they order theirs?
   4. Do I keep several heap arrays of PageDescriptor** that describe the table differently based on priority and ordering?
 */
PageDescriptorEntry* kalloc_page(uint32_t pagesize) {
  return (void*)0;
#if 0
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

  // Needed for the for-loop check, hole / 2 is 0 for hole=1
  kpt_entries[0] = entry;
  
  for (; entry.start < kpt_entries[hole / 2].start; hole /= 2) {
	kpt_entries[hole] = kpt_entries[hole / 2];
  }

  kpt_entries[hole] = entry;
  return &kpt_entries[hole];
#endif
}

uint32_t kalloc_page_get_free(PageDescriptorEntry* e) {
  uint32_t highest_set_bit_index = _asm_bsr(e->used_sections_bitmap);
  return e->size / 32 * (highest_set_bit_index - 1); // Return number of available sections in page
}

PageDescriptorEntry* kalloc_get_page(uint32_t index) {
  return (void*)0;
#if 0
  if (index >= *kpt_entry_count) {
	kPanic("[kalloc_get_page]: Index out of bounds");
	return (void*)0;
  }

  return &kpt_entries[index + 1]; // Index 0 is never used or accessible
#endif
}
