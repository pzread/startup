ARCH = x86_64

CC = ../gcc/gcc/bin/gcc
CFLAGS32 = -masm=intel -fno-builtin -fno-zero-initialized-in-bss -I include -O2
CFLAGS = $(CFLAGS32) -mcmodel=large
LD = ld
OBJCOPY	= objcopy
PAD = python tool/pad.py

default:
	$(MAKE) kernel
	$(MAKE) uefi_loader

	rm *.o

legacy:
	nasm -f elf32 src/legacy_boot.asm -o legacy_boot.o
	$(CC) $(CFLAGS32) -m32 -c src/legacy_loader.c -o legacy_loader.o
	$(LD) -m elf_i386 -T legacy_loader.ld -o legacy_loader.img legacy_boot.o legacy_loader.o
	$(PAD) legacy_loader.img 2560 

	cat legacy_loader.img kernel.img > startup.img
	rm legacy_loader.img

uefi_loader:
	$(CC) $(CFLAGS) -mno-red-zone -mno-mmx -mno-sse -fpic -Wall -fshort-wchar -fno-strict-aliasing -fno-merge-constants -ffreestanding -fno-stack-protector -fno-stack-check -I/usr/include/efi -I/usr/include/efi/x86_64 -I/usr/include/efi/protocol -c src/uefi_loader.c -o uefi_loader.o
	$(LD) -nostdlib --no-undefined -T /usr/lib64/elf_$(ARCH)_efi.lds -shared -Bsymbolic -L/usr/lib64 /usr/lib64/crt0-efi-$(ARCH).o uefi_loader.o -o uefi_loader.so -lefi -lgnuefi $(shell $(CC) -print-libgcc-file-name)
	$(OBJCOPY) -j .text -j .sdata -j .data -j .dynamic -j .dynsym -j .rel -j .rela -j .reloc --target=efi-app-$(ARCH) uefi_loader.so uefi_loader.efi

	rm *.so

kernel:
	$(CC) $(CFLAGS) -m64 -c lib/bitop.c -o bitop.o
	$(CC) $(CFLAGS) -m64 -c lib/list.c -o list.o
	$(CC) $(CFLAGS) -m64 -c lib/std.c -o std.o
	$(CC) $(CFLAGS) -m64 -c src/mm.c -o mm.o
	$(CC) $(CFLAGS) -m64 -c src/graphic.c -o graphic.o
	$(CC) $(CFLAGS) -m64 -c src/kernel_entry.c -o kernel_entry.o
	$(CC) $(CFLAGS) -m64 -c src/kernel.c -o kernel.o
	$(LD) -m elf_x86_64 -T kernel.ld -o kernel.img kernel_entry.o kernel.o bitop.o list.o std.o mm.o graphic.o
	$(PAD) kernel.img 32768
