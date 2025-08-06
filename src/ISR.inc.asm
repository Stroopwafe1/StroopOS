%macro isr 2
global _isr%1	
_isr%1:
	cli
	
	%ifn %2
	push byte 0
	%endif

	push byte %1
	jmp isr_common
%endmacro

%assign i 0
%rep 32
	%if (i == 8) || (i >= 10 && i <= 14)
	isr i, 1
	%else
	isr i, 0
	%endif
%assign i i+1
%endrep	

extern _fault_handler
isr_common:
	pusha
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10   ; Load the Kernel Data Segment descriptor!
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp   ; Push us the stack
    push eax
    mov eax, _fault_handler
    call eax       ; A special call, preserves the 'eip' register
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8     ; Cleans up the pushed error code and pushed ISR number
    iret           ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!
