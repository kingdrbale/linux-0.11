

BOOTSEG = 0x07c0

ROOT_DEV = 0x306


entry _start
_start:
    mov ax,cs
    mov ds,ax
    mov es,ax
    mov ss,ax
    mov sp,#0xFF00

    mov ah,#0x03
    xor bh,bh
    int 0x10
    mov ax,#0x1301
    mov bx,#0x000f
    mov cx,#0x0016

    push ax
    mov ax,#BOOTSEG
    mov es,ax
    pop ax
    mov bp,#Boot_Message
    int 0x10

Boot_Message: 
.byte 13,10
.ascii "Booting System ..."
.byte 13,10

.org 508
root_dev: .word ROOT_DEV
boot_flag:.word 0xAA55

