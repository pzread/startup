CC = gcc
CFLAGS = -masm=intel -fno-builtin -fno-zero-initialized-in-bss -I include
LD = ld
PAD = python tool/pad.py

build_img:
	cat loader.img kernel.img > startup.img

loader:
	nasm -f elf32 src/entry.asm -o entry.o
	$(CC) $(CFLAGS) -m32 -c src/loader.c -o loader.o
	$(LD) -m elf_i386 -T loader.ld -o loader.img entry.o loader.o
	$(PAD) loader.img 2560 

kernel:
	$(CC) $(CFLAGS) -m64 -c lib/std.c -o std.o
	$(CC) $(CFLAGS) -m64 -c src/mm.c -o mm.o
	$(CC) $(CFLAGS) -m64 -c src/kernel.c -o kernel.o
	$(LD) -m elf_x86_64 -T kernel.ld -o kernel.img kernel.o std.o mm.o
	$(PAD) kernel.img 16384

default:
	$(MAKE) loader
	$(MAKE) kernel
	$(MAKE) build_img

	rm loader.img
	rm kernel.img
	rm *.o
