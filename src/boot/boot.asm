ORG 0x7c00
BITS 16

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

_start:
    jmp short start
    nop

times 33 db 0 

start: 
    jmp 0: start2

start2:
    cli ; disable interrupts 
    mov ax, 0x00
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00
    sti ; enable the interrupts


.load_protected: 
    cli 
    lgdt[gdt_descriptor]
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    jmp CODE_SEG:load32 ; switches to the code selector and jumps to load32 abs address


; Global Descriptor Table 
; Information to the CPU about the memory segments

gdt_start: 

gdt_null: 
    dd 0x0
    dd 0x0 

; offset 0x8
gdt_code:     ; CS POINTS TO THIS   These are the default values for the segment flags and details I am going to implement paging later
    dw 0xffff       ;Segment limit first 0-15 bits 
    dw 0            ;Base first 0-15 bits 
    db 0            ;Base 16-23 bits
    db 0x9a         ;Access byte 
    db 11001111b    ;High 4 bit flags and the low 4 bit flags
    db 0            ;Base 24-31 bits

; offset 0x10 
gdt_data:           ; DS SS ES FS GS 
    dw 0xffff       ;Segment limit first 0-15 bits 
    dw 0            ;Base first 0-15 bits 
    db 0            ;Base 16-23 bits
    db 0x92         ;Access byte 
    db 11001111b    ;High 4 bit flags and the low 4 bit flags
    db 0            ;Base 24-31 bits

gdt_end: 

gdt_descriptor: 
    dw gdt_end - gdt_start - 1 ; size
    dd gdt_start               ; offset


[BITS 32] ; everything under here is seen as 32 bit code 

load32: 
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov ebp, 0x00200000 ;BASE POINTER
    mov esp, ebp

    ;Enable the A20 line 
    in al, 0x92
    or al, 2
    out 0x92, al

    jmp $

times 510-($ - $$) db 0 
dw 0xAA55


