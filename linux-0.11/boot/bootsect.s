

BOOTSEG = 0x07c0
INITSEG = 0x9000
SETUPSEG= 0x9020
SETUPLEN= 0x0001

ROOT_DEV = 0x306


entry _start
_start:
# === self-move and set sregs ===
    mov ax,#BOOTSEG
    mov ds,ax
    xor si,si
    mov ax,#INITSEG
    mov es,ax
    xor di,di
    mov cx,#0x0200
    rep
    movsb               
    # 0000:7c11

    # 9000:0018
    jmpi go,#INITSEG    

go: mov ax,cs
    mov ds,ax
    mov es,ax
    mov ss,ax
    mov sp,#0xFF00

load_setup:
    mov dx,#0x0000
    mov cx,#0x0002
    mov bx,#0x0200
    mov ax,#0x0200+SETUPLEN
    int 0x13

    jnc ok_load_setup
    mov dx,#0x0000
    mov ax,#0x0000
    int 0x13
    j load_setup

ok_load_setup:
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

    jmpi 0,SETUPSEG

Boot_Message: 
.byte 13,10
.ascii "Booting System ..."
.byte 13,10

.org 508
root_dev: .word ROOT_DEV
boot_flag:.word 0xAA55

