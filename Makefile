all: test_iso

StroopOS.iso: grub.cfg StroopOS.bin
	mkdir -p build/iso/boot/grub
	cp StroopOS.bin build/iso/boot/StroopOS.bin
	cp grub.cfg build/iso/boot/grub/grub.cfg
	grub-mkrescue -d $(GRUB2_CROSS_PC) -o StroopOS.iso build/iso

debug: StroopOS.iso StroopOS.bin
	qemu-system-i386 -cdrom StroopOS.iso -s -S &
	gdb -ex 'target remote localhost:1234' StroopOS.bin

test: StroopOS.bin
	qemu-system-i386 -kernel StroopOS.bin

test_iso: StroopOS.iso
	qemu-system-i386 -cdrom StroopOS.iso

StroopOS.bin: kernel.o boot.o linker.ld
	i686-elf-gcc -T linker.ld -o StroopOS.bin -ffreestanding -ggdb -nostdlib boot.o kernel.o -lgcc

kernel.o: kernel.c
	i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -ggdb -Wall -Wextra

boot.o: boot.asm
	yasm -DCONSOLE_MODE -gdwarf2 -felf32 boot.asm -o boot.o
