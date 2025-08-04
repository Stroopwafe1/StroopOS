MBALIGN equ 1 << 0			; align loaded (Multiboot) modules on page boundaries
MEMINFO equ 1 << 1			; provide memory mapping
MBFLAGS equ MBALIGN | MEMINFO
MAGIC equ 0x1BADB002		; multiboot header
CHECKSUM equ -(MAGIC + MBFLAGS)

; Declare a multiboot header that marks the program as a kernel. These are magic
; values that are documented in the multiboot standard. The bootloader will
; search for this signature in the first 8 KiB of the kernel file, aligned at a
; 32-bit boundary. The signature is in its own section so the header can be
; forced to be within the first 8 KiB of the kernel file.
section .multiboot
align 4
	dd MAGIC
	dd MBFLAGS
	dd CHECKSUM

section .bss
	align 16
stack_bottom:
	resb 16384 					; Reserve 16 KiB for the stack
stack_top:	

section .text
global _start;:function (_start.end - _start)

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
