ENTRY(NickelMain)

KERNEL_BASE = 0x40000000;
BOOT_HEADER_OFFSET = 0X4000;                                                        /* offset of struct nickel_boot_header in the binary */

SECTIONS {
    . = KERNEL_BASE;
    
    .text : {
        *(.multiboot)
        *(.text)
    }

    .boot_header : ALIGN(BOOT_HEADER_OFFSET) {
        *(.boot_header)
    }

    .rodata : {
        *(.rodata)
    }

    .data : {
        *(.data)
    }

    .bss : {
        *(.bss)
    }
}