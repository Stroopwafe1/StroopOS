all: test_iso

StroopOS.iso: grub.cfg StroopOS.bin
	mkdir -p build/iso/boot/grub
	cp StroopOS.bin build/iso/boot/StroopOS.bin
	cp grub.cfg build/iso/boot/grub/grub.cfg
	grub-mkrescue -d $(GRUB2_CROSS_PC) -o StroopOS.iso build/iso

test: StroopOS.bin
	qemu-system-i386 -kernel StroopOS.bin

test_iso: StroopOS.iso
	qemu-system-i386 -cdrom StroopOS.iso

StroopOS.bin: kernel.o boot.o linker.ld
	i686-elf-gcc -T linker.ld -o StroopOS.bin -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc

kernel.o: kernel.c
	i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

boot.o: boot.asm
	yasm -felf32 boot.asm -o boot.o
