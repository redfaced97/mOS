[bits 32]

section .bss
align 16
stack_bottom:
    resb 16384
stack_top:

section .text
align 4

; =========================
; Multiboot
; =========================
dd 0x1BADB002
dd 0x00
dd -(0x1BADB002 + 0x00)

global start

extern kinit
global generic_stub
global irq0_stub
global irq1_stub
extern generic_interrupt_handler

start:
    cli
    mov esp, stack_top
    call kinit
    hlt

; =========================
; GDT flush
; =========================
global gdt_flush
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

; =========================
; ISR MACROS
; =========================

%macro ISR_NOERR 1
global isr%1
isr%1:
    cli
    push dword 0
    push dword %1
    jmp isr_common
%endmacro

%macro ISR_ERR 1
global isr%1
isr%1:
    cli
    push dword %1
    jmp isr_common
%endmacro

; =========================
; COMMON HANDLER
; =========================

isr_common:
    pusha

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov eax, esp
    push eax

    call generic_interrupt_handler

    add esp, 4
    popa

    add esp, 8
    sti
    iretd

; =========================
; EXCEPTIONS 0–31
; =========================


section .note.GNU-stack