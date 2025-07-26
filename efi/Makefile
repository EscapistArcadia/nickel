GNU_EFI_INCLUDE := $(GNU_EFI_DIR)/inc
GNU_EFI_ARCH_DIR := $(GNU_EFI_DIR)/$(ARCH)
# GNU_EFI_LIB := $(GNU_EFI_ARCH_DIR)/lib $(GNU_EFI_ARCH_DIR)/gnuefi
GNU_EFI_LINKER := $(GNU_EFI_DIR)/gnuefi/elf_$(ARCH)_efi.lds
GNU_EFI_CRT0 := $(GNU_EFI_ARCH_DIR)/gnuefi/crt0-efi-$(ARCH).o
GNU_EFI_FLAGS := -lgnuefi -lefi

CFLAGS += -fpic # position-independent
CFLAGS += -ffreestanding # no standard C library
CFLAGS += -fshort-wchar # 16-bit wide char
CFLAGS += -fno-stack-protector -fno-stack-check
CFLAGS += -Wall -Wextra -Werror -Wshadow \
			-Wno-unused-parameter -Wno-unused-function -Wno-unused-variable \
			-Wswitch-default -Wold-style-definition -Wtype-limits \
			-Wpointer-arith -Waddress -Winline -Waggregate-return
# CFLAGS += -mno-red-zone -maccumulate-outgoing-args
CFLAGS += -D__NICKEL__ -D__nickel__ -DNICKEL_$(ARCH) \
			-DKERNEL_ADDRESS=$(KERNEL_ADDRESS) \
# 			-DNICKEL_HEADER_OFFSET=$(NICKEL_HEADER_OFFSET) \
# 			-DNICKEL_BOOT_MAGIC=$(NICKEL_BOOT_MAGIC) \
# 			-DNICKEL_VERSION=$(NICKEL_VERSION)
CFLAGS += -I $(GNU_EFI_INCLUDE) \
			-I $(KERNEL_INCLUDE)/bootproto

LDFLAGS += -shared -Bsymbolic
LDFLAGS += -nostdlib -znocombreloc
LDFLAGS += -L $(GNU_EFI_ARCH_DIR)/lib
LDFLAGS += -L $(GNU_EFI_ARCH_DIR)/gnuefi
LDFLAGS += -T $(GNU_EFI_LINKER)

SRCS := $(abspath $(wildcard *.c))
OBJS := $(SRCS:.c=.o)

SHARED_OBJ := boot.so

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

boot: $(OBJS)
	$(LD) $(LDFLAGS) $(GNU_EFI_CRT0) $(OBJS) -o $(SHARED_OBJ) $(GNU_EFI_FLAGS)
	$(OBJCOPY) -j .text -j .sdata -j .data -j .rodata -j .dynamic -j .dynsym  -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc --target efi-app-$(ARCH) --subsystem=10 $(SHARED_OBJ) $(BOOTABLE_EFI)

clean:
	rm -rf $(OBJS) $(SHARED_OBJ) $(BOOTABLE_EFI)
