
CFLAGS	=-m32 -g -Wall -O2 -fomit-frame-pointer 

ROOT_DEV = 

all : BootImage

BootImage : tools/build boot/bootsect boot/setup
	tools/build boot/bootsect boot/setup none $(ROOT_DEV) > Image
	sync

boot/bootsect : boot/bootsect.s
	as86 -0 -a -o boot/bootsect.o boot/bootsect.s
	ld86 -0 -s -o boot/bootsect   boot/bootsect.o

boot/setup: boot/setup.s
	as86 -0 -a -o boot/setup.o boot/setup.s
	ld86 -0 -s -o boot/setup   boot/setup.o

tools/build : tools/build.c
	gcc $(CFLAGS) -o tools/build tools/build.c

clean:
	clear
	rm -f tools/build boot/bootsect boot/bootsect.o
