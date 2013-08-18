    global isr_null

    BITS 84
    section .text

isr_null:
    push rsi
    pop rsi
    iretq
