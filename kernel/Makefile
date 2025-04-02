CFLAGS += -Wall -Wextra -Werror -Wredundant-decls -Wshadow \
			-Wno-unused-parameter -Wno-unused-function -Wno-unused-variable \
			-Wmissing-declarations -Wmissing-field-initializers \
			-Wswitch-default -Wold-style-definition -Wtype-limits \
			-Wpointer-arith -Waddress -Winline -Waggregate-return
CFLAGS += -ffreestanding -fpic -fno-stack-protector -fno-stack-check
CFLAGS += -I $(KERNEL_INCLUDE) -I $(ARCH_INCLUDE)
CFLAGS += -D__NICKEL__ -D__nickel__ -DNICKEL_$(ARCH)
CFLAGS += -g -O0
ifeq ($(ARCH), x86_64)
CFLAGS += -mno-red-zone -maccumulate-outgoing-args
endif

# LDFLAGS += -shared -Bsymbolic
LDFLAGS += -T $(LINKER) # -shared -Bsymbolic

SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)
LINKER := linker.ld

all: kernel

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(KERNEL_ELF)
	$(OBJCOPY) -O binary $(KERNEL_ELF) $(KERNEL_EXECUTABLE)
	# $(LD) $(LDFLAGS) $(OBJS) -o $(KERNEL_EXECUTABLE)
	# $(OBJCOPY) -O binary $(KERNEL_EXECUTABLE) $(KERNEL_EXECUTABLE)

clean:
	rm -rf $(OBJS) $(KERNEL_ELF) $(KERNEL_EXECUTABLE)
