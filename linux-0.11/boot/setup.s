entry _start
_start:
mov ax,cs
mov es,ax

mov ah,#0x03
xor bh,bh
int 0x10
mov cx,#0x0013
mov bx,#0x000f
mov ax,#0x1301

mov bp,#msg2
int 0x10

msg2:
.byte 13,10
.ascii "This is SETUP !"
.byte 13,10


