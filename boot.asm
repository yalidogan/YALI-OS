ORG 0x7c00
BITS 16

start: 
    mov si, message
    call print 
    jmp $

print: 
.loop:
    mov bx, 0 
    lodsb # bu command al registerina depoluyo point edilen chari 
    cmp al, 0 # 0 mi diye bakar degilse devam eder
    je .done
    call print_char # bitmediyse print eder al'deki chari 
    jmp .loop
.done:
    ret 


print_char: 
    mov ah, 0eh 
    int 0x10  # bu da bios commandi bir sey basmak icin 
    ret
message: db 'Hello World!', 0 # bu simdi al registerinda depolu yani her char nereye point ediyosa 

times 510-($ - $$) db 0 # 510 byte degilse kalanlari sifirla doldurmak icin bu da
dw 0xAA55