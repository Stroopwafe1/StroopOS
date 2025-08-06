%macro irq 1
global _irq%1	
_irq%1:
	cli
	push byte 0
	push byte %1
	jmp irq_common
%endmacro

%assign i 0
%rep 16
	irq i
%assign i i+1
%endrep

extern _irq_handler

irq_common:
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp
    push eax
    mov eax, _irq_handler
    call eax
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    iret
