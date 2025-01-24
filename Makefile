ARCH := x86_64

AS := $(ARCH)-elf-as
CC := $(ARCH)-elf-gcc
LD := $(ARCH)-elf-ld

AS_FLAGS := -gstabs

ARCH_PATH := arch/$(ARCH)

all: bootloader filesys run



bootloader: $(ARCH_PATH)/boot/entry.s
	$(AS) -o boot.o $(ARCH_PATH)/boot/entry.s -gstabs
	$(AS) -o loader.o $(ARCH_PATH)/boot/loader.s -gstabs
	$(LD) -o boot.bin --oformat binary -Ttext 0x7C00 boot.o
	$(LD) -o loader.bin --oformat binary -Ttext 0x8000 loader.o
ifdef DEBUG
	$(LD) -o boot.elf -Ttext 0x7C00 boot.o
	$(LD) -o loader.elf -Ttext 0x8000 loader.o
endif

filesys:
	dd if=/dev/zero of=filesys.img bs=512 count=262144
	mkfs.fat -F 32 -R 32 filesys.img
	dd if=boot.bin of=filesys.img bs=512 count=1 conv=notrunc
	dd if=boot.bin of=filesys.img bs=512 count=1 seek=6 conv=notrunc
	dd if=loader.bin of=filesys.img count=1 seek=8 conv=notrunc
	mcopy -i filesys.img loader.bin "::NICKEL.EXE"

run:
ifdef DEBUG
	qemu-system-x86_64 -drive file=filesys.img,format=raw -s -S
else
	qemu-system-x86_64 -drive file=filesys.img,format=raw
endif

clean:
	rm -rf *.o *.bin *.elf *.img
