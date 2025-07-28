ENTRY(NickelMain)

KERNEL_BASE = 0x40000000;
BOOT_HEADER_OFFSET = 0xECEB;                                                        /* offset of struct nickel_boot_header in the binary */

SECTIONS {
    . = KERNEL_BASE;
    
    .text : {
        *(.multiboot)
        *(.text)
    }

    .boot_header : AT(KERNEL_BASE + BOOT_HEADER_OFFSET) {
        KEEP(*(.boot_header))
    }

    .rodata : {
        *(.rodata)
    }

    .data : {
        __ld_data_start = .;
        *(.data)
        *(.data.rel*)
        *(.data.rel.*)
        __ld_data_end = .;
    }
    __ld_data_lma = LOADADDR(.data);

    .bss : {
        *(.bss)
    }
}