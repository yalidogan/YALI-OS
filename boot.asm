ORG 0
BITS 16
_start:
    jmp short start
    nop

times 33 db 0 

start: 
    jmp 0x7c0: start2

start2:
    cli ; disable interrupts 
    mov ax, 0x7c0 
    mov ds, ax
    mov es, ax
    mov ax, 0x00
    mov ss, ax
    mov sp, 0x7c00
    sti ; enable the interrupts

    mov ah, 2 ; read sector command
    mov al, 1 ; sectors to read
    mov ch, 0 ; cylinder number
    mov cl, 2 ; sector number 
    mov dh, 0 ; head number 
    mov bx, buffer
    int 0x13
    jc error ; if the carry flag is set it will jump to the error

    mov si, buffer
    call print
    jmp $

error: 
    mov si, error_message
    call print
    jmp $

print: 
.loop:
    mov bx, 0 
    lodsb 
    cmp al, 0 
    je .done
    call print_char 
    jmp .loop
.done:
    ret 


print_char: 
    mov ah, 0eh 
    int 0x10  
    ret

error_message: db 'Failed to load sector', 0

times 510-($ - $$) db 0 
dw 0xAA55

buffer: 
