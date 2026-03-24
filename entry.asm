bits 32

section .bss
align 16
stack_bottom:
    resb 16384
stack_top:

section .text
    ;multiboot spec
    align 4
    dd 0x1BADB002               ; magic Multiboot
    dd 0x00                     ; flags
    dd -(0x1BADB002 + 0x00)     ; checksum

global start

global irq0_stub
global irq1_stub
global gdt_flush
global generic_stub

extern kinit
extern IRQ0_handler
extern keyboard_handler_main

start:
    cli
    mov esp, stack_top
    call kinit
    hlt

%macro IRQ_HANDLER 1
global %1_stub
%1_stub:
    pushad

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    extern %1_handler_main
    call %1_handler_main
    popad
    iretd
%endmacro


irq0_stub:
    pushad
    mov ax, 0x10
    mov ds, ax
    mov es, ax

    call IRQ0_handler


    popad
    iretd

irq1_stub:
    pushad
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    call keyboard_handler_main
    popad
    iretd

generic_stub:
    pushad

    mov ax, 0x10
    mov ds, ax
    mov es, ax

    extern generic_interrupt_handler
    call generic_interrupt_handler

    popad
    iretd
    

gdt_flush:
    mov eax, [esp + 4]
    lgdt [eax]
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:.flush
.flush:
    ret




section .note.GNU-stack
