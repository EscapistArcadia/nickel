ENTRY(NickelMain)

SECTIONS {
    . = 0x40000000;
    
    .text : ALIGN(4K) {
        *(.multiboot)
        *(.text)
    }

    .rodata : ALIGN(4K) {
        *(.rodata)
    }

    .data : ALIGN(4K) {
        *(.data)
    }

    .bss : ALIGN(4K) {
        *(.bss)
    }
}