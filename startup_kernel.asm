%define MAX_PROCESSOR	64

%define GDT_BASE	0xA000
%define GDT_CODE	0x8
%define GDT_DATA	0x10
%define GDT_TSSD_BASE	0x18
%define GDT_SIZE	0x18 + MAX_PROCESSOR * 16

%define TSSD_BASE	GDT_BASE + 0x18
%define TSS_BASE	0x1000
%define	IOBITMAP_BASE	0xDFFF

%define PT_BASE		0x10000
%define STACK_TOP	0x40000000

%define VMEM_ADDR	0x500
%define IOAPIC_ADDR	0x508
%define LAPIC_ADDR	0x510
%define MP_COUNT	0x518

    global kernel
    extern video_drawtext
    extern std_sprintf

    BITS 64
    section .data
tssd:
    dd	00000000000000000000000001100111b
    dd	00000000000000001000100100000000b
    dd	00000000000000000000000000000000b
    dd	00000000000000000000000000000000b
gdt_ptr:
    dw	GDT_SIZE - 1
    dq	GDT_BASE
idt_ptr:
    dw  0xFFF
    dq  0xC000

outy:	dq  100
fmta:	db  '[Startup] I am CPU %u',0
fmtb:	db  '[Startup] Found %u cpu(s)',0

    section .text
kernel:
    mov ax,GDT_DATA
    mov fs,ax
    mov gs,ax
    mov rsp,STACK_TOP

    mov al,0xFF			;Disable PIC
    out 0xA1,al
    out 0x21,al

    mov rdi,TSSD_BASE		;Init TSSD
    mov rdx,TSS_BASE << 16
    mov rcx,MAX_PROCESSOR
.loInitTSSD:
    mov	rax,[tssd]
    add rax,rdx
    mov [rdi],rax
    mov	rax,[tssd + 8]
    mov [rdi + 8],rax
    add rdi,16
    add rdx,0x68 << 16
    loop .loInitTSSD

    mov rdi,TSS_BASE		;Init TSS
    mov bx,0xCFFF
    mov rcx,MAX_PROCESSOR
.loInitTSS:
    mov rdx,rcx
    xor rax,rax
    mov rcx,102 / 4
    rep stosd
    mov word [rdi],bx
    add rdi,2
    sub bx,0x68
    mov rcx,rdx
    loop .loInitTSS

    mov rdi,IOBITMAP_BASE	;Init IO bitmap 64K bits
    mov rcx,8192 / 4
    rep stosd
    mov byte [rdi],0xFF

    lgdt [gdt_ptr]		;Reload new GDT

    mov ax,GDT_TSSD_BASE
    ltr	ax			;Load TSS 0

    mov rdi,0xC000		;Init ISR
    mov rcx,256
.loISR:
    mov rax,0x00008E0000080000
    add rax,isr_null
    stosq
    mov rax,0x0000000000000000
    stosq
    loop .loISR 

    mov rdi,32
    mov si,isr_32
    call setisr
    mov rdi,33
    mov si,isr_33
    call setisr
    mov rdi,39
    mov si,isr_spurious
    call setisr

    lidt [idt_ptr]		;Load IDT

    mov rsi,0xE0000		;Load ACPI data
    mov rcx,0x20000
.loFindRDSPa:
    cmp dword [rsi],0x20445352	;sign "RSD PTR "
    jne .cloFindRDSPa
    mov rdx,rcx
    xor al,al
    mov rcx,20
.loCheckRDSPa:
    add al,[rsi + rcx - 1]
    loop .loCheckRDSPa
    mov rcx,rdx
    test al,al
    jz .eFindRDSP
.cloFindRDSPa:
    add rsi,16
    loop .loFindRDSPa

    xor rsi,rsi
    mov esi,[0x40E]
    shl esi,4
    mov rcx,64
.loFindRDSPb:
    cmp dword [rsi],0x20445352	;sign "RSD PTR "
    jne .cloFindRDSPb
    mov rdx,rcx
    xor al,al
    mov rcx,20
.loCheckRDSPb:
    add al,[rsi + rcx - 1]
    loop .loCheckRDSPb
    mov rcx,rdx
    test al,al
    jz .eFindRDSP
.cloFindRDSPb:
    add rsi,16
    loop .loFindRDSPb

.eFindRDSP:

    mov al,[rsi + 15]
    test al,al
    jnz .elifACPIver
    xor rax,rax			;ACPI version = 1.0
    mov eax,[rsi + 16]
    mov rsi,rax
    xor rcx,rcx
    mov ecx,[rsi + 4] 
    sub rcx,36
    shr rcx,2
    add rsi,36
    xor rax,rax
.loACPIa:
    mov eax,[rsi]
    cmp dword [eax],0x43495041	;sign "APIC"
    je .eloACPIa
    add rsi,4
    loop .loACPIa
.eloACPIa:
    mov rsi,rax
    jmp .eifACPIver
.elifACPIver:
    mov rsi,[rsi + 24]		;ACPI version >= 2.0
    xor rcx,rcx
    mov ecx,[rsi + 4] 
    sub rcx,36
    shr rcx,3
    add rsi,36
.loACPIb:
    mov rax,[rsi]
    cmp dword [eax],0x43495041	;sign "APIC"
    je .eloACPIb
    add rsi,8
    loop .loACPIb
.eloACPIb:
    mov rsi,rax
.eifACPIver:

    xor rax,rax
    mov rdi,rsi
    mov eax,[rsi + 4] 
    add rdi,rax 
    add rsi,44
.loMADT:
    mov al,[rsi]
    cmp al,1
    je .eloMADT			;Suppose only has one I/O APIC
.cloMADT:
    xor rax,rax
    mov al,[rsi + 1]
    add rsi,rax
    cmp rsi,rdi
    jne .loMADT
.eloMADT:
    xor rax,rax
    mov eax,[rsi + 4]
    mov [IOAPIC_ADDR],rax	;Init I/O APIC register address

    xor rax,rax    
    mov rcx,0x1B
    rdmsr
    and eax,0xFFFFF000
    mov [LAPIC_ADDR],rax	;Init Local APIC register address

    call init_lapic
    sti				;Enable interrupt

    mov rsi,[LAPIC_ADDR]
    mov word [MP_COUNT],1
    mov eax,0x0
    mov [rsi + 0x310],eax
    mov eax,0xC4500
    mov [rsi + 0x300],eax	;Send INIT IPI
    hlt
    mov eax,apstart
    shr eax,12
    add eax,0xC4600
    mov [rsi + 0x300],eax	;Send Start IPI

    mov rcx,16
.loWaitCPUs:
    hlt
    loop .loWaitCPUs
	


    mov rbp,rsp
    sub rsp,0x80

    mov qword [rbp - 8],0

    sub rsp,0x28
    lea rdi,[rbp - 128]
    mov rsi,fmta
    lea rdx,[rbp - 8]
    call std_sprintf 
    add rsp,0x28

    sub rsp,0x28
    mov rdi,100
    mov rsi,18
    lock xadd [outy],rsi
    lea rdx,[rbp - 128]
    call video_drawtext
    add rsp,0x28

    add rsp,0x80
    mov rbp,rsp
    sub rsp,0x80

    xor rax,rax
    mov ax,[MP_COUNT]
    mov [rbp - 8],rax

    sub rsp,0x28
    lea rdi,[rbp - 128]
    mov rsi,fmtb
    lea rdx,[rbp - 8]
    call std_sprintf 
    add rsp,0x28

    sub rsp,0x28
    mov rdi,100
    mov rsi,18
    lock xadd [outy],rsi
    lea rdx,[rbp - 128]
    call video_drawtext
    add rsp,0x28

    add rsp,0x80




    jmp .end

.end:
    hlt
    jmp .end

setisr:
    push rax
    push rdi

    mov rax,0x00008E0000080000
    add ax,si
    shl rdi,4
    add rdi,0xC000
    mov [rdi],rax
    xor rax,rax
    mov [rdi + 8],rax

    pop rdi
    pop rax
    ret

isr_null:
    push rsi
    mov rsi,[LAPIC_ADDR]
    mov dword [rsi + 0xB0],0x0	;Set EOI register
    pop rsi
    iretq

isr_spurious:
    iretq

isr_32:				;APIC Timer IRQ
    push rsi
    mov rsi,[LAPIC_ADDR]
    mov dword [rsi + 0xB0],0x0	;Set EOI register
    pop rsi
    iretq

isr_33:				;PS/2 Keyboard IRQ
    push rsi
    mov rsi,[LAPIC_ADDR]
    mov dword [rsi + 0xB0],0x0	;Set EOI register
    pop rsi
    iretq

init_lapic:
    push rax
    push rsi

    mov rsi,[LAPIC_ADDR]	;Init Local APIC
    mov	dword [rsi + 0x80],0x0	;Set Task Priority Register
    mov eax,[rsi + 0xD0]	;Set Local destination register
    mov eax,0xFFFFFFFF
    mov [rsi + 0xD0],eax
    mov dword [rsi + 0xE0],0xFFFFFFFF	;Set Destination format register

    mov eax,0x10000		;Disable
    mov [rsi + 0x320],eax	;LVT Timer Register
    mov [rsi + 0x350],eax	;LVT LINT0 Register
    mov [rsi + 0x360],eax	;LVT LINT1 Register
    mov dword [rsi + 0x340],0x400	;LVT Performance Counter Register, Delivery Mode NMI

    mov ecx,0x1B		;Enable global APIC
    rdmsr
    or eax,0x800
    wrmsr
    mov dword [rsi + 0xF0],0x100 + 39	;Set Spurious-Interrupt Vector Register, APIC Software Enable + IRQ 39
    
    mov dword [rsi + 0x3E0],0xB		;Init APIC Timer
    mov dword [rsi + 0x320],0x20020	;IRQ 32
    mov dword [rsi + 0x380],0x100000

    pop rsi
    pop rax
    ret




    BITS 16
    section .aptext
apstart:
    mov eax,0x10000
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
    jmp GDT_CODE:apstart64

    BITS 64
apstart64:
    mov ax,GDT_DATA
    mov fs,ax
    mov gs,ax

    xor rdx,rdx
    mov dx,1
    lock xadd [MP_COUNT],dx
    mov r15,rdx			;r15 = processor index

    mov rax,r15
    shl rax,22			;Stack size 4MB
    mov rsp,STACK_TOP
    sub rsp,rax
    
    mov rax,r15
    shl ax,4
    add ax,GDT_TSSD_BASE
    ltr	ax			;Load TSS

    lidt [idt_ptr]		;Load IDT
    
    call init_lapic
    sti				;Enable interrupt

    hlt
    



    mov rbp,rsp
    sub rsp,0x80

    mov [rbp - 8],r15

    sub rsp,0x28
    lea rdi,[rbp - 128]
    mov rsi,fmta
    lea rdx,[rbp - 8]
    call std_sprintf 
    add rsp,0x28

    sub rsp,0x28
    mov rdi,100
    mov rsi,18
    lock xadd [outy],rsi
    lea rdx,[rbp - 128]
    call video_drawtext
    add rsp,0x28

    add rsp,0x80




.end:
    hlt
    jmp .end
