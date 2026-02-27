bits 32

section .bss
align 16
stack_bottom:
    resb 16384 ; 16 КБ под стек
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
extern timer_handler
extern keyboard_handler_main

start:
    cli                     ; отключаем прерывания
    mov esp, stack_top      ; настраиваем стек
    call kinit              ; переходим в C‑ядро
    hlt                     ; halt системы

%macro IRQ_HANDLER 1
global %1_stub
%1_stub:
    pushad          ; Сохраняем EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    extern %1_handler_main ; Или как называются твои функции в C
    call %1_handler_main   ; Для таймера это будет irq0_handler_main

    popad
    iretd           ; Используем 32-битную версию возврата
%endmacro

    ; --- Исправляем твои стабы вручную для надежности ---

irq0_stub:
    pushad
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    call timer_handler


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
