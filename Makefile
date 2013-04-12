default:
	nasm -f bin startup_loader.asm -o startup_loader.bin
	nasm -f bin startup_kernel.asm -o startup_kernel.bin
	cat startup_loader.bin > startup.img
	cat startup_kernel.bin >> startup.img
	./padding startup.img 16384
