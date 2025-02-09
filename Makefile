# architecture specification
ARCH ?= x86_64

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
# KERNEL_DIR := $(PWD)/kernel
# FS_DIR := $(PWD)/fs

BOOTABLE_EFI := $(PWD)/boot.efi
FILE_SYSTEM_IMAGE := $(PWD)/filesys.img

ifeq ($(ARCH), x86_64)
	BOOTABLE_ELF_DEST := bootx64.efi
	UEFI_BIOS := OVMF.fd # mannually downloaded from my linux vm
	# UEFI_BIOS := /opt/homebrew/share/qemu/edk2-x86_64-code.fd
else ifeq ($(ARCH), aarch64)
	BOOTABLE_ELF_DEST := bootaa64.efi
	UEFI_BIOS := /opt/homebrew/share/qemu/edk2-aarch64-code.fd
else
	$(error "Unsupported Architecture: %(ARCH)")
endif

export ARCH CC LD AS OBJCOPY
export GNU_EFI_DIR ARCH_DIR EFI_SRC_DIR BOOTABLE_EFI FILE_SYSTEM_IMAGE

all: efi_boot filesys

efi_boot:
	$(MAKE) -C $(EFI_SRC_DIR)

filesys:
	dd if=/dev/zero of=$(FILE_SYSTEM_IMAGE) bs=512 count=262144
	mkfs.vfat -F 32 $(FILE_SYSTEM_IMAGE)
	mmd -i $(FILE_SYSTEM_IMAGE) ::EFI
	mmd -i $(FILE_SYSTEM_IMAGE) ::EFI/BOOT
	mcopy -i $(FILE_SYSTEM_IMAGE) $(BOOTABLE_EFI) ::EFI/BOOT/$(BOOTABLE_ELF_DEST)

run:
ifeq ($(ARCH), x86_64)
	qemu-system-x86_64 -drive format=raw,file=$(FILE_SYSTEM_IMAGE) -bios $(UEFI_BIOS)
else ifeq ($(ARCH), aarch64)
	qemu-system-aarch64 -drive format=raw,file=$(FILE_SYSTEM_IMAGE) -bios $(UEFI_BIOS) -machine virt -cpu cortex-a72 -m 512M
else
	$(error "Unsupported Architecture: %(ARCH)")
endif

clean:
	$(MAKE) -C $(EFI_SRC_DIR) clean
	rm -rf $(FILE_SYSTEM_IMAGE)
