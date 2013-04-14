default:
	nasm -f elf64 startup_loader.asm -o startup_loader.o
	nasm -f elf64 startup_kernel.asm -o startup_kernel.o
	gcc -m64 -c startup_lib.c -o startup_lib.o
	ld -T startup.ld -o startup.img startup_loader.o startup_kernel.o startup_lib.o
	./padding startup.img 16384
