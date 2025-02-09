# architecture specification
ARCH ?= aarch64

# gnu-efi directory
GNU_EFI_DIR := $(PWD)/../gnu-efi
# GNU_EFI_ARCH_DIR := $(GNU_EFI_LIB)/$(ARCH)
# GNU_EFI_LIB := $(GNU_EFI_ARCH_DIR)/lib $(GNU_EFI_ARCH_DIR)/gnuefi
# GNU_EFI_LINKER := $(GNU_EFI_DIR)/gnuefi/elf_$(ARCH)_efi.lds
# GNU_EFI_CRT0 := $(GNU_EFI_ARCH_DIR)/gnuefi/crt0-efi-$(ARCH).o
# GNU_EFI_FLAGS := -lgnuefi -lefi

# TODO: edk2?

# compiler specification
CROSS_COMPILE := $(ARCH)-elf-
CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)ld
AS := $(CROSS_COMPILE)as
OBJCOPY := $(CROSS_COMPILE)objcopy
GDB := $(CROSS_COMPILE)gdb

ARCH_DIR := $(PWD)/arch/$($(ARCH))
EFI_SRC_DIR := $(PWD)/efi

BOOTABLE_EFI := boot.efi
FILE_SYSTEM_IMAGE := filesys.img

export ARCH GNU_EFI_DIR CC LD AS OBJCOPY ARCH_DIR EFI_SRC_DIR

all:
	$(MAKE) -C $(EFI_SRC_DIR)
	$(MAKE) filesys

filesys:
	@dd if=/dev/zero of=$(FILE_SYSTEM_IMAGE) bs=512 count=262144
	@mkfs.vfat -F 32 $(FILE_SYSTEM_IMAGE)
	@mmd -i $(FILE_SYSTEM_IMAGE) ::EFI
	@mmd -i $(FILE_SYSTEM_IMAGE) ::EFI/BOOT
	@mcopy -i $(FILE_SYSTEM_IMAGE) $(BOOTABLE_EFI) ::EFI/BOOT/BOOTAA64.EFI

run:
	qemu-system-aarch64 -machine virt -cpu cortex-a72 -m 512M -bios /usr/share/qemu-efi-aarch64/QEMU_EFI.fd -drive format=raw,file=filesys.img

clean:
	$(MAKE) -C $(EFI_SRC_DIR) clean
	rm -rf $(FILE_SYSTEM_IMAGE)
