

.code16
.globl ap_startup, ap_startup_end

// .org 0x8000
ap_startup:
    // int3
    cli
    xorw %ax, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs

    movw $0x8000, %bp
    movw %bp, %sp

    movl $0xECEBCAFE, %eax

    // cli
    // pushfw
    // popw %ax
    // andw $0xFEFF, %ax
    // pushw %ax
    // popfw

    // jmp ap_startup_halt

// // enable protected mode
//     movl %cr0, %eax
//     orl $0x80000000, %eax
//     movl %eax, %cr0

//     lgdt ap_gdt32_reg

// // setup stacks & prepare for lret
//     // movw $0x7C00, %bp
//     // movw %bp, %sp
//     ljmpl $0x10, $ap_startup_32

// .code32
// ap_startup_32:
//     cli
//     movl $0x10, %eax
//     movl %eax, %ds
//     movl %eax, %es
//     movl %eax, %fs
//     movl %eax, %gs

//     movl $0x8000, %ebp
//     movl %ebp, %esp

ap_startup_halt:
    hlt
    jmp ap_startup_halt

.align 16
ap_gdt32_desc:
    .quad 0
    .quad 0x00CF9A000000FFFF
    .quad 0x00CF92000000FFFF
    .quad 0x00CFFA000000FFFF
    .quad 0x00CFF2000000FFFF
ap_gdt32_desc_end:

.align 4
    .word 0
ap_gdt32_reg:
    .word ap_gdt32_desc_end - ap_gdt32_desc - 1
    .long ap_gdt32_desc

ap_startup_end:
