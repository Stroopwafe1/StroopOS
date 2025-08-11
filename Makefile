BUILDDIR := build
PROJECTDIRS := src

SRCFILES := $(shell find $(PROJECTDIRS) -type f -name "*.c" -o -name "boot.asm")
HEADERFILES := $(shell find $(PROJECTDIRS) -type f -name "\*.h")
OBJFILES := $(addprefix $(BUILDDIR)/, $(addsuffix .o, $(notdir $(SRCFILES))) Lat2-Terminus16.psfu.o)

VPATH := $(dir $(SRCFILES))

INCLUDES := $(addprefix -I, $(dir $(SRCFILES)))

CFLAGS := -c -std=gnu99 -ffreestanding -ggdb -Wall -Wextra $(INCLUDES)
QEMU_OPTIONS := -m 128 -usb -smp 1 -serial stdio -d cpu_reset,guest_errors,pcall

CC := i686-elf-gcc
AS := nasm

ASFLAGS := $(INCLUDES) -g -felf32

.PHONY: all test test_iso debug clean

all: $(BUILDDIR) $(BUILDDIR)/StroopOS.bin StroopOS.iso test_iso

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean: $(BUILDDIR)
	rm -rf $(BUILDDIR)/*

debug: $(BUILDDIR)/iso/StroopOS.iso $(BUILDDIR)/StroopOS.bin
	qemu-system-i386 -cdrom $(BUILDDIR)/iso/StroopOS.iso -s -S $(QEMU_OPTIONS) &
	gdb -ex 'target remote localhost:1234' $(BUILDDIR)/StroopOS.bin

test: $(BUILDDIR)/StroopOS.bin
	qemu-system-i386 -kernel $(BUILDDIR)/StroopOS.bin

test_iso: $(BUILDDIR)/iso/StroopOS.iso
	qemu-system-i386 -cdrom $(BUILDDIR)/iso/StroopOS.iso $(QEMU_OPTIONS)

$(BUILDDIR)/StroopOS.bin: $(OBJFILES) src/linker.ld
	$(CC) -T src/linker.ld -o $(BUILDDIR)/StroopOS.bin -ffreestanding -ggdb -nostdlib $(OBJFILES) -lgcc

$(BUILDDIR)/Lat2-Terminus16.psfu.o: Lat2-Terminus16.psfu
	objcopy -O elf32-i386 -B i386 -I binary Lat2-Terminus16.psfu $(BUILDDIR)/Lat2-Terminus16.psfu.o

$(BUILDDIR)/%.c.o: %.c
	$(CC) $(CFLAGS) $< -o $@

$(BUILDDIR)/%.asm.o: %.asm
	$(AS) $< $(ASFLAGS) -o $@

StroopOS.iso: src/grub.cfg $(BUILDDIR)/StroopOS.bin
	mkdir -p $(BUILDDIR)/iso/boot/grub
	cp $(BUILDDIR)/StroopOS.bin $(BUILDDIR)/iso/boot/StroopOS.bin
	cp src/grub.cfg $(BUILDDIR)/iso/boot/grub/grub.cfg
	grub-mkrescue -d $(GRUB2_CROSS_PC) -o StroopOS.iso $(BUILDDIR)/iso
	mv StroopOS.iso $(BUILDDIR)/iso/StroopOS.iso
