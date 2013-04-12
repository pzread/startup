    BITS 64
    ORG 0x7E00

    section .data
gdt.ptr:
	dw  0x28
	dq  0xA000

idt.ptr:
	dw  0xFFF
	dq  0xC000

msga:	db  '[Startup] Found 0 cpu(s)',0
msgb:	db  '[Startup] Hello World',0

    section .bss
apic_base:  resq    1
mp_count:   resd    1

    section .text
start:
    mov al,0xFF	;Disable PIC
    out 0xA1,al
    out 0x21,al

    xor rax,rax
    mov rdi,0x1000   ;Init TSS and IOMap
    mov rcx,102 / 4
    rep stosd
    mov word [rdi],0xCFFF
    mov rdi,0xDFFF
    mov rcx,8192 / 4 ;IOMap 64K bits
    rep stosd
    mov byte [rdi],0xFF

    mov ax,0x18	;gdt.tssd
    ltr	ax

    mov rdi,0xC000 ;Init ISR
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

    lidt [idt.ptr]

		    ;Init APIC
    xor rax,rax
    mov rcx,0x1B    ;APIC_BASE
    rdmsr
    and eax,0xFFFFF000
    mov [apic_base],rax

    mov rsi,[apic_base]
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

    mov ecx,0x1B    ;Enable global APIC
    rdmsr
    or eax,0x800
    wrmsr
    mov dword [rsi + 0xF0],0x100 + 39	;Set Spurious-Interrupt Vector Register, APIC Software Enable + IRQ 39

    sti	;Enable interrupt

    mov eax,0xB	;APIC Timer
    mov dword [rsi + 0x3E0],0xB
    mov dword [rsi + 0x320],0x20000 + 32
    mov dword [rsi + 0x380],0x100000

    mov dword [mp_count],0
    mov eax,0x0
    mov [rsi + 0x310],eax
    mov eax,0xC4500
    mov [rsi + 0x300],eax
    hlt
    mov eax,apstart
    shr eax,12
    add eax,0xC4600
    mov [rsi + 0x300],eax

    mov rcx,16
.loWaitCPUs:
    hlt
    loop .loWaitCPUs
	
    mov rsi,0xE0000
    mov rcx,0x20000
.loFindRDSPa:
    cmp dword [rsi],0x20445352
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
    cmp dword [rsi],0x20445352
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
    xor rax,rax		;ACPI version = 1.0
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
    cmp dword [eax],0x43495041
    je .eloACPIa
    add rsi,4
    loop .loACPIa
.eloACPIa:
    mov rsi,rax
    jmp .eifACPIver
.elifACPIver:
    mov rsi,[rsi + 24]	;ACPI version >= 2.0
    xor rcx,rcx
    mov ecx,[rsi + 4] 
    sub rcx,36
    shr rcx,3
    add rsi,36
.loACPIb:
    mov rax,[rsi]
    cmp dword [eax],0x43495041
    je .eloACPIb
    add rsi,8
    loop .loACPIb
.eloACPIb:
    mov rsi,rax
.eifACPIver:

    mov rdi,rsi
    xor rax,rax
    mov eax,[rsi + 4] 
    add rdi,rax 
    add rsi,44
    xor rax,rax
.loMADT:
    mov al,[rsi]
    cmp al,1
    je .eloMADT	;Suppose only has one I/O APIC
    mov al,[rsi + 1]
    add rsi,rax
    cmp rsi,rdi
    jne .loMADT
.eloMADT:
    xor rax,rax
    mov eax,[rsi + 4]
    mov rsi,rax	;I/O APIC Address

    mov dword [rsi],0x13
    mov dword [rsi + 0x10],0xFF000000
    mov dword [rsi],0x12
    mov dword [rsi + 0x10],100000100001b

    mov eax,[mp_count]
    add eax,'1'
    mov byte [msga + 16],al
    mov rdi,100
    mov rsi,100
    mov rdx,msga
    call drawtext

    mov rdi,100
    mov rsi,118
    mov rdx,msgb
    call drawtext

    jmp end

end:
    hlt
    jmp end

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
    mov rsi,[apic_base]
    mov dword [rsi + 0xB0],0x0	;Set EOI register
    pop rsi
    iretq

isr_spurious:
    iretq

isr_32:
    push rsi
    mov rsi,[apic_base]
    mov dword [rsi + 0xB0],0x0	;Set EOI register
    pop rsi
    iretq

isr_33:
    push rsi
    mov rsi,[apic_base]
    mov dword [rsi + 0xB0],0x0	;Set EOI register
    pop rsi
    iretq

drawchar:
    push rax
    push rcx
    push rdx
    push rdi
    push rsi
    push r8
    push r9
    push r10
    pushfq

    mov r8,rdi
    mov r9,rsi

    mov rax,rdx
    mov rcx,16
    mul rcx
    add rax,0x7F000
    mov rsi,rax

    mov rdi,[0x500]
    mov r10,rdi

    mov rax,r9
    mov rcx,1024
    mul rcx
    add rax,r8
    mov rcx,3
    mul rcx
    add r10,rax
    mov rdx,15
.loY:
    mov rcx,8
    mov rdi,r10
    mov al,[rsi]
.loX:
    bt ax,7
    jnc .eifX
    mov word [rdi],0xD9D9
    mov byte [rdi + 2],0xD9
.eifX:
    add rdi,3
    shl ax,1
    loop .loX

    add rsi,1
    add r10,3072
    dec rdx
    test rdx,rdx
    jnz .loY

    popfq
    pop r10
    pop r9
    pop r8
    pop rsi
    pop rdi
    pop rdx
    pop rcx
    pop rax
    ret

drawtext:
    push rax
    push rdx
    push rdi

    mov rax,rdx
    xor rdx,rdx
.lo:
    mov dl,[rax]
    test dl,dl
    jz .elo
    call drawchar    
    add rdi,8
    inc rax
    jmp .lo
.elo:

    pop rdi
    pop rdx
    pop rax
    ret

    BITS 16
    section .aptext align=4096
apstart:
    cli
    lock inc dword [mp_count]

    mov eax,0x10000
    mov cr3,eax

    mov eax,cr4	;Enable PAE
    or eax,1 << 5
    mov cr4,eax

    mov ecx,0xC0000080	;Enable LME
    rdmsr
    or eax,1 << 8
    wrmsr
    
    mov eax,cr0	;Enable long mode
    or eax,1 << 31 | 1 << 0
    mov cr0,eax

    lgdt [gdt.ptr]  ;Load GDT

    jmp 0x8:apstart64

    BITS 64
apstart64:
    mov ax,0x10	;gdt.data
    mov fs,ax
    mov gs,ax
    mov rsp,0x3C000000
    
    hlt

    ;mov ax,
    ;ltr	ax
