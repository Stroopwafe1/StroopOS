all: test_iso

build/iso/StroopOS.iso: src/grub.cfg build/StroopOS.bin
	mkdir -p build/iso/boot/grub
	cp build/StroopOS.bin build/iso/boot/StroopOS.bin
	cp src/grub.cfg build/iso/boot/grub/grub.cfg
	grub-mkrescue -d $(GRUB2_CROSS_PC) -o build/iso/StroopOS.iso build/iso

debug: build/iso/StroopOS.iso build/StroopOS.bin
	qemu-system-i386 -cdrom build/iso/StroopOS.iso -s -S &
	gdb -ex 'target remote localhost:1234' build/StroopOS.bin

test: build/StroopOS.bin
	qemu-system-i386 -kernel build/StroopOS.bin

test_iso: build/iso/StroopOS.iso
	qemu-system-i386 -cdrom build/iso/StroopOS.iso

build/StroopOS.bin: build/kernel.o build/boot.o src/linker.ld
	i686-elf-gcc -T src/linker.ld -o build/StroopOS.bin -ffreestanding -ggdb -nostdlib build/boot.o build/kernel.o -lgcc

build/kernel.o: src/kernel.c
	i686-elf-gcc -c src/kernel.c -o build/kernel.o -std=gnu99 -ffreestanding -ggdb -Wall -Wextra

build/boot.o: src/boot.asm
	yasm -DCONSOLE_MODE -gdwarf2 -felf32 src/boot.asm -o build/boot.o
