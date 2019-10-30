
CFLAGS	=-m32 -g -Wall -O2 -fomit-frame-pointer 

ROOT_DEV = 

all : BootImage

BootImage : boot/bootsect tools/build
	tools/build boot/bootsect none none $(ROOT_DEV) > Image
	sync

boot/bootsect : boot/bootsect.s
	as86 -0 -a -o boot/bootsect.o boot/bootsect.s
	ld86 -0 -s -o boot/bootsect	  boot/bootsect.o

tools/build : tools/build.c
	gcc $(CFLAGS) -o tools/build tools/build.c

clean:
	clear
	rm -f tools/build boot/bootsect boot/bootsect.o
