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
# CFLAGS += -mno-red-zone -maccumulate-outgoing-args
CFLAGS += -I $(GNU_EFI_INCLUDE)

LDFLAGS += -shared -Bsymbolic
LDFLAGS += -L $(GNU_EFI_ARCH_DIR)/lib
LDFLAGS += -L $(GNU_EFI_ARCH_DIR)/gnuefi
LDFLAGS += -T $(GNU_EFI_LINKER)

SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)

SHARED_OBJ := boot.so
BOOT_EFI := $(PWD)/boot.efi

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

boot: $(OBJS)
	$(LD) $(LDFLAGS) $(GNU_EFI_CRT0) $(OBJS) -o $(SHARED_OBJ) $(GNU_EFI_FLAGS)
	$(OBJCOPY) -j .text -j .sdata -j .data -j .rodata -j .dynamic -j .dynsym  -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc --target efi-app-$(ARCH) --subsystem=10 $(SHARED_OBJ) $(BOOT_EFI)

clean:
	rm -rf $(OBJS) $(SHARED_OBJ) $(BOOT_EFI)
