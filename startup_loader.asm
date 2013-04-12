;   Memory map
;   0x500	-	0xFFF	    Global data
;   0x1000	-	0x7BFF	    TSS
;   0x7C00	-	0x7DFF	    Loader
;   0x7E00	-	0x8FFF	    Kernel
;   0x9000	-	0x9FFF	    AP
;   0xA000	-	0xBFFF	    Descriptor table
;   0xC000	-	0xCFFF	    ISR
;   0xDFFF	-	0xFFFF	    IO bitmap
;   0x10000	-	0x13FFF	    Page table
;   0x7F000	-	0x7FFFF	    VGA font

;   Global data
;   0x0		-	0x7	    Viedo Memory Base

    ORG 0x7C00
    jmp 0x0000:start

    BITS 16
start:
    xor ax,ax
    mov ds,ax
    mov es,ax
    mov ss,ax
    xor ecx,ecx
    xor edi,edi
    xor esi,esi

    mov ax,0x2401   ;Enable A20
    int 0x15

    mov si,dapack   ;Load bottom half
    mov ah,0x42
    mov dl,0x80
    int 0x13

    mov ax,0x1130   ;Load VGA font
    mov bh,0x6
    int 0x10
    mov ax,es
    mov ds,ax
    mov ax,0x7F00
    mov es,ax
    mov si,bp
    xor di,di
    mov cx,256 * 16 / 4
    rep movsd
    xor ax,ax
    mov es,ax
    mov ds,ax
    
    mov di,0x1000   ;Init video mode info memory (Temp use TSS memory)
    mov cx,2048
    xor eax,eax
    rep stosd

    mov ax,0x4F00   ;Get Video Info
    mov di,0x1000
    int 0x10
    mov esi,[0x1000 + 0xE]
.loVMode:
    mov cx,[esi]
    cmp cx,0xFFFF
    je .eloVMode

    mov ax,0x4F01   ;Get Video Mode Info
    mov di,0x2000
    int 0x10
    mov ax,[0x2000]	    ;ModeAttributes
    and ax,0x90		    ;Graphic Mode & Linear frame buffer
    cmp ax,0x90
    jne .cloVMode
    mov ax,[0x2000 + 0x12]  ;XResolution
    cmp ax,1024
    jne .cloVMode
    mov ax,[0x2000 + 0x14]  ;YResolution
    cmp ax,768
    jne .cloVMode
    mov al,[0x2000 + 0x19]  ;BitsPerPixel
    cmp al,24
    jne .cloVMode
    ;mov al,[0x2000 + 0x1B]  ;MemoryModel
    ;cmp al,6
    ;jne .cloVMode

    mov ax,0x4F02
    mov bx,cx
    or bx,0x4000
    int 0x10

    mov eax,[0x2000 + 0x28]  ;PhysBasePtr
    mov [0x500],eax
    xor eax,eax
    mov [0x504],eax

    jmp .eloVMode
.cloVMode:
    add esi,2
    jmp .loVMode
.eloVMode:
    xor esi,esi

    mov si,gdt	    ;Init Descriptor table
    mov di,0xA000
    mov cx,gdt.size / 4
    rep movsd

    mov ax,0x1000   ;Init page table
    mov es,ax   
    mov di,0x0
    mov cx,4096
    xor eax,eax
    rep stosd

    mov di,0x0
    mov dword [es:di],0x11003	    ;PML4E0
    add di,0x1000
    mov dword [es:di],0x12003	    ;PDPE0
    mov dword [es:di + 24],0x13003  ;PDPE1

    mov di,0x2000		    ;PDE0
    mov ebx,0x00000083
    mov cx,512
.loPDEa:
    mov dword [es:di],ebx 
    add ebx,0x200000
    add di,8
    loop .loPDEa
    mov di,0x3000		    ;PDE3
    mov ebx,0xC0000083
    mov cx,512
.loPDEd:
    mov dword [es:di],ebx 
    add ebx,0x200000
    add di,8
    loop .loPDEd

    xor ax,ax
    mov es,ax
    mov eax,0x10000
    mov cr3,eax

    mov eax,cr4	;Enable PAE
    or eax,1 << 5
    mov cr4,eax

    mov ecx,0xC0000080	;Enable LME
    rdmsr
    or eax,1 << 8
    wrmsr
    
    cli

    mov eax,cr0	;Enable long mode
    or eax,1 << 31 | 1 << 0
    mov cr0,eax

    lgdt [gdt.ptr]  ;Load GDT
    jmp gdt.code:start64

dapack:
    db	0x10
    db	0
    dw	17
    dd	0x00007E00
    dd	1
    dd	0

gdt:
.null:	equ $ - gdt
    dw	0000000000000000b
    dw	0000000000000000b
    dw	0000000000000000b
    dw	0000000000000000b
.code:	equ $ - gdt
    dw	0000000000000000b
    dw	0000000000000000b
    dw	1001100000000000b
    dw	0000000000100000b
.data:	equ $ - gdt
    dw	0000000000000000b
    dw	0000000000000000b
    dw	1001000000000000b
    dw	0000000000000000b
.tssd:	equ $ - gdt
    dd	00010000000000000000000001100111b   ;TSS Base 0x1000
    dd	00000000000000001000100100000000b
    dd	00000000000000000000000000000000b
    dd  00000000000000000000000000000000b
.size:	equ $ - gdt
.ptr:
    dw	$ - gdt- 1
    dq	0xA000

    BITS 64
start64:
    mov ax,gdt.data
    mov fs,ax
    mov gs,ax
    mov rsp,0x40000000
    
    jmp 0x7E00

padding:
    times 510 - ($ - $$) db 0
    dw	0xAA55
