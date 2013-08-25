    extern lapic_base
    global isr_null
    global isr_spurious

    BITS 64
    section .text

isr_null:
    push rsi
    mov rsi,lapic_base
    mov rsi,[rsi]
    mov dword [rsi + 0xB0],0x0	;Set EOI register
    pop rsi
    iretq

isr_spurious:
    iretq
