BUILDDIR=build

OBJS = \
$(BUILDDIR)/kernel.o \
$(BUILDDIR)/boot.o \
$(BUILDDIR)/tty.o \
$(BUILDDIR)/idt.o \
$(BUILDDIR)/isrs.o \
$(BUILDDIR)/irq.o \
$(BUILDDIR)/timer.o \
$(BUILDDIR)/keyboard.o \
$(BUILDDIR)/psf_render.o \
$(BUILDDIR)/Lat2-Terminus16.o

CFLAGS = -c -std=gnu99 -ffreestanding -ggdb -Wall -Wextra

QEMU_OPTIONS = -m 128 -usb -smp 1 -serial stdio -d cpu_reset -d int -d guest_errors

.PHONY: all test test_iso debug

all: $(OBJS) build/StroopOS.bin StroopOS.iso test_iso

debug: build/iso/StroopOS.iso build/StroopOS.bin
	qemu-system-i386 -cdrom build/iso/StroopOS.iso -s -S $(QEMU_OPTIONS) &
	gdb -ex 'target remote localhost:1234' build/StroopOS.bin

test: build/StroopOS.bin
	qemu-system-i386 -kernel build/StroopOS.bin

test_iso: build/iso/StroopOS.iso
	qemu-system-i386 -cdrom build/iso/StroopOS.iso $(QEMU_OPTIONS)

build/StroopOS.bin: $(OBJS) src/linker.ld
	i686-elf-gcc -T src/linker.ld -o build/StroopOS.bin -ffreestanding -ggdb -nostdlib $(OBJS) -lgcc

build/boot.o: src/*.asm
	nasm -isrc/ -g -felf32 src/boot.asm -o build/boot.o

build/Lat2-Terminus16.o: Lat2-Terminus16.psfu
	objcopy -O elf32-i386 -B i386 -I binary Lat2-Terminus16.psfu build/Lat2-Terminus16.o

build/%.o: src/%.c src/*.h
	i686-elf-gcc $(CFLAGS) $< -o $@ 

StroopOS.iso: src/grub.cfg build/StroopOS.bin
	mkdir -p build/iso/boot/grub
	cp build/StroopOS.bin build/iso/boot/StroopOS.bin
	cp src/grub.cfg build/iso/boot/grub/grub.cfg
	grub-mkrescue -d $(GRUB2_CROSS_PC) -o StroopOS.iso build/iso
	mv StroopOS.iso build/iso/StroopOS.iso
