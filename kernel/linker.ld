ENTRY(NickelMain)

KERNEL_BASE = 0x40000000;
BOOT_HEADER_OFFSET = 0xECEB;                                                        /* offset of struct nickel_boot_header in the binary */

SECTIONS {
    . = KERNEL_BASE;
    
    .text : {
        *(.text)
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

    .boot_header (KERNEL_BASE + BOOT_HEADER_OFFSET) : AT(KERNEL_BASE + BOOT_HEADER_OFFSET) {
        KEEP(*(.boot_header))
    }
}