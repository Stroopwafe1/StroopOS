MBALIGN equ 1 << 0			; align loaded (Multiboot) modules on page boundaries
MEMINFO equ 1 << 1			; provide memory mapping
MBFLAGS equ MBALIGN | MEMINFO
MBARCH equ 0					; 0 means 32-bit (protected) mode i386
MBHEADER_LENGTH equ mb_header_end - mb_header_begin	
MAGIC equ 0xE85250D6		; multiboot header
CHECKSUM equ -(MAGIC + MBARCH + MBHEADER_LENGTH)


MB_TAG_FLAGS_TYPE equ 4
MB_TAG_FLAGS_SIZE equ 12

MB_TAG_FB_TYPE equ 5
MB_TAG_FB_SIZE equ 20

MB_TAG_TERM_TYPE equ 0
MB_TAG_TERM_SIZE equ 8
	
MB_TAG_OPTIONAL equ 0
; Declare a multiboot header that marks the program as a kernel. These are magic
; values that are documented in the multiboot standard. The bootloader will
; search for this signature in the first 8 KiB of the kernel file, aligned at a
; 32-bit boundary. The signature is in its own section so the header can be
; forced to be within the first 8 KiB of the kernel file.
section .multiboot
align 4
mb_header_begin:
	dd MAGIC
	dd MBARCH
	dd MBHEADER_LENGTH
	dd CHECKSUM

align 8
	%ifdef CONSOLE_MODE
	dw MB_TAG_FLAGS_TYPE
	dw MB_TAG_OPTIONAL
	dd MB_TAG_FLAGS_SIZE
	dd MBFLAGS
	%endif

align 8
	dw MB_TAG_FB_TYPE
	dw MB_TAG_OPTIONAL
	dd MB_TAG_FB_SIZE
	dd 1024
	dd 768
	dd 32

align 8
	dw MB_TAG_TERM_TYPE
	dw MB_TAG_OPTIONAL
	dw MB_TAG_TERM_SIZE
mb_header_end:	

section .bss
	align 16
stack_bottom:
	resb 16384 					; Reserve 16 KiB for the stack
stack_top:	

section .text
global _start:function (_start.end - _start)

_start:
	mov esp, stack_top
	;; Initialise processor state here

	extern kernel_main
	call kernel_main

	cli
.hang:
	hlt
	jmp .hang
.end:
