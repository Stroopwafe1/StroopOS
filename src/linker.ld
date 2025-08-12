/* The bootloader will look at this image and start execution at the symbol
   designated as the entry point. */
ENTRY(_start)

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
	. = 2M;

	_kernel_start = .;
	.multiboot : {
		*(.multiboot)
		*(.multiboot_text)
	}

	. += 0xC0000000;
	_kernel_addr_begin = .;
	.text ALIGN(4K) : AT (ADDR (.text) - 0xC0000000)
	{
		*(.text)
	}

	/* Read-only data. */
	.rodata ALIGN(4K) : AT (ADDR (.rodata) - 0xC0000000)
	{
		*(.rodata)
	}

	/* Read-write data (initialized) */
	.data ALIGN(4K) : AT (ADDR (.data) - 0xC0000000)
	{
		*(.data)
	}

	/* Read-write data (uninitialized) and stack */
	.bss ALIGN(4K) : AT (ADDR (.bss) - 0xC0000000)
	{
		*(COMMON)
		*(.bss)
		*(.mmu_pages)
	}
	_kernel_end = .;

	/* The compiler may produce other sections, by default it will put them in
	   a segment with the same name. Simply add stuff here as needed. */
}
