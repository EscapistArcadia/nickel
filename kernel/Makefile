CFLAGS += -Wall -Wextra -Werror -Wredundant-decls -Wshadow \
			-Wno-unused-parameter -Wno-unused-function -Wno-unused-variable \
			-Wmissing-declarations -Wmissing-field-initializers \
			-Wswitch-default -Wold-style-definition -Wtype-limits \
			-Wpointer-arith -Waddress -Winline -Waggregate-return
CFLAGS += -ffreestanding -fpic -fno-stack-protector -fno-stack-check
CFLAGS += -I $(KERNEL_INCLUDE) -I $(ARCH_INCLUDE)
CFLAGS += -D__NICKEL__ -D__nickel__ -DNICKEL_$(ARCH) \
			-DNICKEL_HEADER_OFFSET=$(NICKEL_HEADER_OFFSET) \
			-DKERNEL_ADDRESS=$(KERNEL_ADDRESS) \
			-DNICKEL_BOOT_MAGIC=$(NICKEL_BOOT_MAGIC) \
			-DNICKEL_VERSION=$(NICKEL_VERSION)
CFLAGS += -g -O0
ifeq ($(ARCH), x86_64)
CFLAGS += -mno-red-zone -maccumulate-outgoing-args
endif

# LDFLAGS += -shared -Bsymbolic
LDFLAGS += -T $(LINKER) # -shared -Bsymbolic

SRCS := $(abspath $(wildcard *.c))
OBJS := $(SRCS:.c=.o)
LINKER := linker.ld

all: kernel

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(KERNEL_ELF)
	$(OBJCOPY) -O binary $(KERNEL_ELF) $(KERNEL_EXECUTABLE)

clean:
	rm -rf $(OBJS) $(KERNEL_ELF) $(KERNEL_EXECUTABLE)
