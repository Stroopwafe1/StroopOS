# StroopOS - 32 bit kernel POC

This is a project I started to learn about how Operating Systems operate. 

## Things I have learned:

- [x] How bootloaders recognise a bootable image
- [x] The [Multiboot](https://nongnu.askapache.com/grub/phcoder/multiboot.pdf) Specification
- [x] How Operating Systems stay running and don't just exit immediately
- [x] How Operating Systems draw pixels to the monitor
- [x] Interoperability between C and Assembly
- [x] Implementation of some LibC functions
- [x] Font Rendering
- [x] CPU Interrupts
- [x] How Keyboards work
- [x] Function Pointers in C (I didn't really use them before)

## How to run this project

I should have included an ISO in the [Github Releases](https://github.com/Stroopwafe1/StroopOS/releases)

But if you want to make sure you can also build it:

### Dependencies:
- QEMU
- GCC for i386 ELF
- NASM
- Linux's binutils (specifically objcopy)
- GNU Make

Optional (To make the ISO)
- GRUB2 (specifically grub-mkrescue)

#### NixOS Users:

I included a flake.nix, you can just use `nix develop` to enter the dev environment. All dependencies will be installed

### Build Instructions:
If you want to build the ISO
```sh
$ mkdir -p build
$ make
```

Else
```sh
$ mkdir -p build
$ make test
```
