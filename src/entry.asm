    global entry
    extern main

    BITS 16
    section .text

entry:
    xor ax,ax
    mov ds,ax
    mov es,ax
    mov ss,ax

    mov ax,0x2401		    ;Enable A20
    int 0x15
    
    mov si,diskpack		    ;Load bottom half
    mov ah,0x42
    mov dl,0x80
    int 0x13

    mov sp,0x7DC0		    ;Init stack (2KB)
    jmp 0x0000:main

diskpack:
    db	0x10			    
    db	0
    dw	4			    ;number_of_block
    dd	0x00007E00		    ;buffer_segment:buffer_offset 
    dd	1			    ;lba_number
    dd	0			    

padding:
    times 510 - ($ - $$) db 0
    dw	0xAA55
