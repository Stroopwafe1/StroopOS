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
	
MMU_PAGE_SIZE equ 4096
KERNEL_ADDR_BEGIN equ 0xC0000000
	
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
	resq 16384 					; Reserve 64 KiB for the stack
stack_top:

section .mmu_pages nobits alloc write		; Kernel Page Table
	align MMU_PAGE_SIZE
mmu_page_directory:
	resb MMU_PAGE_SIZE
mmu_page_table1:
	resb MMU_PAGE_SIZE

extern _kernel_start
extern _kernel_end

section .multiboot_text alloc exec
global _start:function
_start:
	;; Set up paging
	mov edi, mmu_page_table1 - KERNEL_ADDR_BEGIN
	xor esi, esi
	mov ecx, 1024				; Counter, we want 1024 page entries in our page table
.loop:
	;; 	cmp esi, _kernel_start	jl .inc_ptr
	cmp esi, (_kernel_end - KERNEL_ADDR_BEGIN) + MMU_PAGE_SIZE
	jge .loop_end

	;;  Map physical address as present, and writable(!)
	mov edx, esi
	or edx, 0x3
	mov [edi], edx
.inc_ptr:
	add esi, MMU_PAGE_SIZE
	add edi, 4
	loop .loop
.loop_end:
	;;  The page table is used at both page directory entry 0 (virtually from 0x0 to 0x3FFFFF) (thus identity mapping the kernel) and page directory entry 768 (virtually from 0xC0000000 to 0xC03FFFFF) (thus mapping it in the  higher half). The kernel is identity mapped because enabling paging does not change the next instruction, which continues to be physical. The CPU would instead page fault if there was no identity mapping.

	;; Map the page table to both virtual addresses 0x00000000 and 0xC0000000.
	mov dword [mmu_page_directory - KERNEL_ADDR_BEGIN + 0], mmu_page_table1 - KERNEL_ADDR_BEGIN + 0x003
	mov dword [mmu_page_directory - KERNEL_ADDR_BEGIN + 768 * 4], mmu_page_table1 - KERNEL_ADDR_BEGIN + 0x003

	;; Self-reference page directory so we can write into it later
	mov dword [mmu_page_directory - KERNEL_ADDR_BEGIN + 1023 * 4], mmu_page_directory - KERNEL_ADDR_BEGIN + 0x03
	
	;; Set CR3 to the page directory
	mov ecx, mmu_page_directory - KERNEL_ADDR_BEGIN
	mov cr3, ecx

	;; Enable paging and the write-protect bit
	mov ecx, cr0
	or ecx, 0x80010000
	mov cr0, ecx

	lea ecx, _start_after_paging
	jmp ecx
section .text
_start_after_paging:
	;; EBX has the multiboot information
	extern multiboot_info
	mov [multiboot_info], ebx
	
	;; Reload CR3 to force a TLB flush
	mov ecx, cr3
	mov cr3, ecx
	
	mov esp, stack_top
	
	cli
	lgdt [gdt_desc]
	call reload_segments
	
	extern kernel_main
	call kernel_main

	cli
.hang:
	hlt
	jmp .hang
.end:

;;; This doesn't work for some reason, investigate it later, but for now it works if the kernel is still identity mapped
global _unmap_identity
_unmap_identity:
	;; 	mov dword [mmu_page_directory - KERNEL_ADDR_BEGIN], 0
	ret

	;; flags dword esp+12
	;; size dword esp+8
	;; memory address dword esp+4
global _map_page
_map_page:	
	mov ebx, cr3
	mov eax, [esp+4]
	shr eax, 22
	mov ecx, eax				; pd index
	xchg ebx, eax

	mov eax, [esp+4]
	shr eax, 12
	and eax, 0x03FF
	mov edx, eax				; pt index

	;; TODO: Figure out how to map this properly
	mov dword [eax + ebx*4], mmu_page_table1
	ret
	
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

global _asm_bsr
_asm_bsr:
	bsr eax, [esp+4]
	ret

global _idt_load
extern _idtp
_idt_load:
	lidt [_idtp]
	ret

%include "ISR.inc.asm"
%include "IRQ.inc.asm"
