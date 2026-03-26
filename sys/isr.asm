; sys/isr.asm
BITS 32
section .text

; --- Глобальные ISR ---
global isr0, isr1, isr2, isr3, isr4, isr5, isr6, isr7
global isr8, isr9, isr10, isr11, isr12, isr13, isr14, isr15
global isr16, isr17, isr18, isr19, isr20, isr21, isr22, isr23
global isr24, isr25, isr26, isr27, isr28, isr29, isr30, isr31
global isr32, isr33, isr34, isr35, isr36, isr37, isr38, isr39
global isr40, isr41, isr42, isr43, isr44, isr45, isr46, isr47

; --- Макросы ---
; IRQ без кода ошибки
%macro ISR_STUB_NOERR 2
; %1 = имя ISR, %2 = номер INT
%1:
    pusha
    push dword 0        ; err = 0
    push dword %2       ; int_no
    extern generic_interrupt_handler
    call generic_interrupt_handler
    add esp, 8
    popa
    iretd
%endmacro

; CPU exceptions (с кодом ошибки)
%macro ISR_STUB_ERR 3
; %1 = имя ISR, %2 = номер INT, %3 = 1 если есть код ошибки
%1:
    pusha
    %if %3
        ; err уже на стеке
    %else
        push dword 0    ; err = 0
    %endif
    push dword %2      ; int_no
    extern generic_interrupt_handler
    call generic_interrupt_handler
    add esp, 8
    popa
    iretd
%endmacro

; --- CPU exceptions 0-31 ---
ISR_STUB_ERR isr0, 0, 0
ISR_STUB_ERR isr1, 1, 0
ISR_STUB_ERR isr2, 2, 0
ISR_STUB_ERR isr3, 3, 0
ISR_STUB_ERR isr4, 4, 0
ISR_STUB_ERR isr5, 5, 0
ISR_STUB_ERR isr6, 6, 0
ISR_STUB_ERR isr7, 7, 0
ISR_STUB_ERR isr8, 8, 1
ISR_STUB_ERR isr9, 9, 1
ISR_STUB_ERR isr10, 10, 1
ISR_STUB_ERR isr11, 11, 1
ISR_STUB_ERR isr12, 12, 1
ISR_STUB_ERR isr13, 13, 1
ISR_STUB_ERR isr14, 14, 1
ISR_STUB_ERR isr15, 15, 0
ISR_STUB_ERR isr16, 16, 0
ISR_STUB_ERR isr17, 17, 0
ISR_STUB_ERR isr18, 18, 0
ISR_STUB_ERR isr19, 19, 0
ISR_STUB_ERR isr20, 20, 0
ISR_STUB_ERR isr21, 21, 0
ISR_STUB_ERR isr22, 22, 0
ISR_STUB_ERR isr23, 23, 0
ISR_STUB_ERR isr24, 24, 0
ISR_STUB_ERR isr25, 25, 0
ISR_STUB_ERR isr26, 26, 0
ISR_STUB_ERR isr27, 27, 0
ISR_STUB_ERR isr28, 28, 0
ISR_STUB_ERR isr29, 29, 0
ISR_STUB_ERR isr30, 30, 0
ISR_STUB_ERR isr31, 31, 0

; --- IRQ 0-15 (32-47) ---
ISR_STUB_NOERR isr32, 32
ISR_STUB_NOERR isr33, 33
ISR_STUB_NOERR isr34, 34
ISR_STUB_NOERR isr35, 35
ISR_STUB_NOERR isr36, 36
ISR_STUB_NOERR isr37, 37
ISR_STUB_NOERR isr38, 38
ISR_STUB_NOERR isr39, 39
ISR_STUB_NOERR isr40, 40
ISR_STUB_NOERR isr41, 41
ISR_STUB_NOERR isr42, 42
ISR_STUB_NOERR isr43, 43
ISR_STUB_NOERR isr44, 44
ISR_STUB_NOERR isr45, 45
ISR_STUB_NOERR isr46, 46
ISR_STUB_NOERR isr47, 47