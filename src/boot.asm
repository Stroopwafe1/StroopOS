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

;; (base, limit, access, flags)
%macro SegmentDescriptor 4
	dd ((%1 & 0xFFFF) | (%2 & 0xFFFF))
	dd ((%1 & 0xFF000000) | (%4 << 20) | (%2 & 0x0F0000) | (%3 << 8) | ((%1 & 0x00FF0000) >> 16))
%endmacro
	
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

section .kpt nobits alloc write		; Kernel Page Table
	align 16
	resd 2
							; Entry Count
							; First Entry PTR

section .text
global _start:function (_start.end - _start)
_start:
	mov esp, stack_top
	;; Initialise processor state here

	cli
	lgdt [gdt_desc]
	call reload_segments
	
	;; EBX has the multiboot information
	extern multiboot_info
	mov [multiboot_info], ebx
	
	extern kernel_main
	call kernel_main

	cli
.hang:
	hlt
	jmp .hang
.end:

align 8	
gdt_begin:	
	;;  NULL Descriptor
	dq 0
	
	;; Kernel mode code segment
	SegmentDescriptor 0, 0xFFFFF, 0x9A, 0xC
	
	;; Kernel mode data segment
	SegmentDescriptor 0, 0xFFFFF, 0x92, 0xC

	;; User Mode code segment
	SegmentDescriptor 0, 0xFFFFF, 0xFA, 0xC

	;; User Mode Data segment
	SegmentDescriptor 0, 0xFFFFF, 0xF2, 0xC
gdt_end:
gdt_desc:
	dw gdt_end - gdt_begin - 1
	dd gdt_begin

reload_segments:
	jmp 0x08:reload_CS
reload_CS:
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	ret

global _outb
_outb:
	mov edx, [esp+4]
	mov eax, [esp+8]
	out dx, al
	ret

global _inb
_inb:
	mov edx, [esp+4]
	in eax, dx
	ret
	
global _idt_load
extern _idtp
_idt_load:
	lidt [_idtp]
	ret

%include "ISR.inc.asm"
%include "IRQ.inc.asm"
