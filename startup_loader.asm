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
;   0x8		-	0xF	    IO APIC register address
;   0x10	-	0x17	    Local APIC register address
;   0x18	-	0x19	    MP count

%define GDT_BASE	0xA000
%define GDT_CODE	0x8
%define GDT_DATA	0x10
%define GDT_SIZE	0x18

%define PT_BASE		0x10000
%define VFONT_BASE	0x7F000

%define VMEM_ADDR	0x500

    global loader
    extern kernel

    BITS 16
    section .text
loader:
    jmp 0x0000:start

start:
    xor ax,ax
    mov ds,ax
    mov es,ax
    mov ss,ax

    mov ax,0x2401		;Enable A20
    int 0x15

    mov si,dapack		;Load bottom half
    mov ah,0x42
    mov dl,0x80
    int 0x13

    mov ax,0x1130		;Load VGA font
    mov bh,0x6
    int 0x10
    mov ax,es
    mov ds,ax
    mov ax,VFONT_BASE >> 4
    mov es,ax
    mov si,bp
    xor di,di
    mov ecx,256 * 16 / 4
    rep movsd
    xor ax,ax
    mov es,ax
    mov ds,ax
    
    mov di,0x1000		;Init video mode info memory (Temp use TSS memory)
    mov ecx,2048
    xor eax,eax
    rep stosd

    mov ax,0x4F00		;Get Video Info
    mov di,0x1000
    int 0x10
    mov esi,[0x1000 + 0xE]
.loVMode:
    mov cx,[esi]
    cmp cx,0xFFFF
    je .eloVMode

    mov ax,0x4F01		;Get Video Mode Info
    mov di,0x2000
    int 0x10
    mov ax,[0x2000]		;ModeAttributes
    and ax,0x90			;Graphic Mode & Linear frame buffer
    cmp ax,0x90
    jne .cloVMode
    mov ax,[0x2000 + 0x12]	;XResolution
    cmp ax,1024
    jne .cloVMode
    mov ax,[0x2000 + 0x14]	;YResolution
    cmp ax,768
    jne .cloVMode
    mov al,[0x2000 + 0x19]	;BitsPerPixel
    cmp al,24
    jne .cloVMode

    mov ax,0x4F02
    mov bx,cx
    or bx,0x4000
    int 0x10

    mov eax,[0x2000 + 0x28]	;PhysBasePtr
    mov [VMEM_ADDR],eax
    xor eax,eax
    mov [VMEM_ADDR + 4],eax

    jmp .eloVMode
.cloVMode:
    add esi,2
    jmp .loVMode
.eloVMode:

    mov si,gdt			;Init Descriptor table
    mov di,GDT_BASE
    mov ecx,GDT_SIZE / 4
    rep movsd

    mov ax,PT_BASE >> 4		;Init page table
    mov es,ax   
    mov di,0x0
    mov ecx,4096
    xor eax,eax
    rep stosd
						
    mov di,0x0
    mov dword [es:di],PT_BASE + 0x1003		;PML4E0
    add di,0x1000
    mov dword [es:di],PT_BASE + 0x2003		;PDPE0
    mov dword [es:di + 24],PT_BASE + 0x3003	;PDPE1

    mov di,0x2000				;PDE0
    mov ebx,0x00000083
    mov ecx,512
.loPDEa:
    mov dword [es:di],ebx 
    add ebx,0x200000
    add di,8
    loop .loPDEa
    mov di,0x3000				;PDE3
    mov ebx,0xC0000083
    mov ecx,512
.loPDEd:
    mov dword [es:di],ebx 
    add ebx,0x200000
    add di,8
    loop .loPDEd

    xor ax,ax
    mov es,ax
    mov eax,PT_BASE
    mov cr3,eax

    mov eax,cr4			;Enable PAE
    or eax,1 << 5
    mov cr4,eax

    mov ecx,0xC0000080		;Enable LME
    rdmsr
    or eax,1 << 8
    wrmsr
    
    cli

    mov eax,cr0			;Enable long mode
    or eax,1 << 31 | 1 << 0
    mov cr0,eax

    lgdt [gdt_ptr]		;Load GDT
    jmp GDT_CODE:kernel

dapack:
    db	0x10
    db	0
    dw	17
    dd	0x00007E00
    dd	1
    dd	0
gdt:
    dw	0000000000000000b   ;null
    dw	0000000000000000b
    dw	0000000000000000b
    dw	0000000000000000b
    dw	0000000000000000b   ;code
    dw	0000000000000000b
    dw	1001100000000000b
    dw	0000000000100000b
    dw	0000000000000000b   ;data
    dw	0000000000000000b
    dw	1001000000000000b
    dw	0000000000000000b
gdt_ptr:
    dw	GDT_SIZE - 1
    dq	GDT_BASE

padding:
    times 510 - ($ - $$) db 0
    dw	0xAA55
