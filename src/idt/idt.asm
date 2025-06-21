section .asm 

; to tell the processor where the idt is 
global idt_load 
idt_load: 
    push ebp
    mov ebp, esp
    
    mov ebx, [ebp + 8]
    lidt[ebx] 
    pop ebp 
    ret