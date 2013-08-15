CC = ~/devel/gcc/gcc/bin/gcc
CFLAGS32 = -masm=intel -fno-builtin -fno-zero-initialized-in-bss -I include -O2
CFLAGS = $(CFLAGS32) -mcmodel=large
LD = ld
PAD = python tool/pad.py
MERGE = python tool/merge.py

default:
	$(MAKE) loader
	$(MAKE) kernel
	$(MAKE) build_img

	rm *.o
	rm *.bin
	rm loader.img
	rm kernel.img

build_img:
	cat loader.img kernel.img > startup.img

loader:
	nasm -f elf32 src/entry.asm -o entry.o
	nasm -f bin src/entry64.asm -o entry64.bin
	$(CC) $(CFLAGS32) -m32 -c src/loader.c -o loader.o
	$(LD) -m elf_i386 -T loader.ld -o loader.img entry.o loader.o
	$(MERGE) loader.img entry64.bin 448
	$(PAD) loader.img 2560 

kernel:
	$(CC) $(CFLAGS) -m64 -c lib/bitop.c -o bitop.o
	$(CC) $(CFLAGS) -m64 -c lib/list.c -o list.o
	$(CC) $(CFLAGS) -m64 -c lib/std.c -o std.o
	$(CC) $(CFLAGS) -m64 -c src/mm.c -o mm.o
	$(CC) $(CFLAGS) -m64 -c src/kernel.c -o kernel.o
	$(LD) -m elf_x86_64 -T kernel.ld -o kernel.img kernel.o bitop.o list.o std.o mm.o
	$(PAD) kernel.img 16384
