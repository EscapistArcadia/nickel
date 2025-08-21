CFLAGS += -Wall -Wextra -Werror -Wredundant-decls -Wshadow \
			-Wno-unused-parameter -Wno-unused-function -Wno-unused-variable \
			-Wmissing-declarations -Wmissing-field-initializers \
			-Wswitch-default -Wold-style-definition -Wtype-limits \
			-Wpointer-arith -Waddress -Winline -Waggregate-return
CFLAGS += -ffreestanding -fno-stack-protector -fno-stack-check \
			-fno-pic -fno-pie -fno-plt
CFLAGS += -I $(KERNEL_INCLUDE) -I $(ARCH_INCLUDE)


ARCH_MACRO := $(shell echo $(ARCH) | tr '[:lower:]' '[:upper:]')
CFLAGS += -D__NICKEL__ -D__nickel__ -DNICKEL_$(ARCH_MACRO) \
			-DKERNEL_ADDRESS=$(KERNEL_ADDRESS) \
# 			-DNICKEL_HEADER_OFFSET=$(NICKEL_HEADER_OFFSET) \
# 			-DNICKEL_BOOT_MAGIC=$(NICKEL_BOOT_MAGIC) \
# 			-DNICKEL_VERSION=$(NICKEL_VERSION)
CFLAGS += -g -O2
ifeq ($(ARCH), x86_64)
CFLAGS += -mno-red-zone -maccumulate-outgoing-args
endif

# LDFLAGS += -shared -Bsymbolic
LDFLAGS += -T $(LINKER)
LDFLAGS += -nostdlib -static 

ASM_SRCS := $(abspath $(shell find $(ARCH_DIR) -name "*.s")) $(abspath $(shell find . -name "*.s"))
SRCS := $(abspath $(shell find $(ARCH_DIR) -name "*.c")) $(abspath $(shell find . -name "*.c"))
OBJS := $(ASM_SRCS:.s=.o) $(SRCS:.c=.o)
LINKER := linker.ld

all: kernel

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(CC) $(CFLAGS) -c $< -o $@

kernel: $(OBJS) $(ASM_OBJS)
	$(LD) $(LDFLAGS) $(OBJS) $(ASM_OBJS) -o $(KERNEL_ELF)
	$(OBJCOPY) -O binary $(KERNEL_ELF) $(KERNEL_EXECUTABLE)

clean:
	rm -rf $(OBJS) $(ASM_OBJS) $(KERNEL_ELF) $(KERNEL_EXECUTABLE)
