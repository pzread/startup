    BITS 64
    section .text

entry64:
    mov rsp,0xFFFF800000007DC0	;Init stack (2KB)
    mov rax,0xFFFF800000010000
    jmp rax
