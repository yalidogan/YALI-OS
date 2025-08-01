ORG 0x7c00
BITS 16

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start


jmp short start
nop

;Header is kind of standard and included in the online sources as well just be vary of the number of bytes and the instruction names 
;FAT16 HEADER 
OEMIdentifier               db 'YALIOS  ' 
BytesPerSector              dw 0x200
SectorsPerCluster           db 0x80 
ReservedSectors             dw 200 
FATCopies                   db 0x02 
RootDirEntries              dw 0x40 
NumSectors                  dw 0x00
MediaType                   db 0xF8
SectorsPerFat               dw 0x100
SectorsPerTrack             dw 0x20 
NumberOfHeads               dw 0x40 
HiddenSectors               dd 0x00
SectorsBig                  dd 0x773594

;Extended BPB ()
DriveNumber                 db 0x80 
WinNTBit                    db 0x00
Signature                   db 0x29 
VolumeID                    dd 0xD105
VolumeIDString              db 'YALIOS BOOT'
SystemIDString              db 'FAT16   '




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

[BITS 32]
load32: 
    mov eax, 1
    mov ecx, 100
    mov edi, 0x0100000
    call ata_lba_read
    jmp CODE_SEG:0x0100000

ata_lba_read: 
    mov ebx, eax ; Backup the LBA
    ;Send the highest 8 bits of the LBA to the hard disk controller 
    shr eax, 24
    or eax, 0xE0 ; Select the master drive not the slave drive 
    mov dx, 0x1F6
    out dx, al
    ; Finished sending the highest 8 bits of the LBA

    ; Send the total sectors to read 
    mov eax, ecx 
    mov dx, 0x1F2
    out dx, al
    ; Finished sending the total sectors to read 

    ; Send more bits of the LBA
    mov eax, ebx ; Restore the LBA 
    mov dx, 0x1F3
    out dx, al
    ; Finished sending more bits of the LBA

    ; Send more bits of the LBA 
    mov dx, 0x1F4
    mov eax, ebx ; Restore the backup LBA 
    shr eax, 8
    out dx, al
    ; Finished sending more bits of the LBA 

    ;Send the upper 16 bits of the LBA 
    mov dx, 0x1F5
    mov eax, ebx ;restore 
    shr eax, 16 
    out dx, al

    ;Finished sendin the upper 16 bits of the LBA 

    mov dx, 0x1F7
    mov al, 0x20
    out dx, al

    ; Read all sectors into memory 
.next_sector: 
    push ecx

; Checking if we need to read
.try_again: 
    mov dx, 0x1F7
    in al, dx 
    test al, 8
    jz .try_again

;We need to read 256 words at a time 
    mov ecx, 256 
    mov dx, 0x1F0
    rep insw ;Reads from the port specified in dx into the place specified in EDI 
    pop ecx 
    loop .next_sector 

    ;End of reading sectors into mem 
    ret 




times 510-($ - $$) db 0 
dw 0xAA55


