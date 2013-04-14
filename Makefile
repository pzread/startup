default:
	nasm -f elf64 startup_loader.asm -o startup_loader.o
	nasm -f elf64 startup_kernel.asm -o startup_kernel.o
	ld -T startup.ld -o startup.img startup_loader.o startup_kernel.o
	./padding startup.img 16384
