#ifndef __ARCH_TYPE_H__
#define __ARCH_TYPE_H__

typedef __INT8_TYPE__       int8_t;
typedef __INT16_TYPE__      int16_t;
typedef __INT32_TYPE__      int32_t;
typedef __INT64_TYPE__      int64_t;
typedef __UINT8_TYPE__      uint8_t;
typedef __UINT16_TYPE__     uint16_t;
typedef __UINT32_TYPE__     uint32_t;
typedef __UINT64_TYPE__     uint64_t;

#define SDAB_ACCESSED                   0b0001
#define SDAB_READ_WRITE                 0b0010
#define SDAB_DIRECTION_CONFORMING       0b0100
#define SDAB_EXECUTABLE                 0b1000

union gdt_entry {
    uint64_t value[2];

    struct {
        uint16_t    limit_15_00;                                                    /* number of accessible bytes from base */
        uint16_t    base_15_00;                                                     /* starting physical address of the segment */
        uint8_t     base_23_16;
        uint8_t     access_byte : 4;
        uint8_t     not_sys_seg : 1;                                                /* system segment (1) or code/data segment (0) */
        uint8_t     dpl         : 2;                                                /* descriptor privilege level */
        uint8_t     present     : 1;                                                /* segment present in memory (1) or not (0) */
        uint8_t     limit_19_16 : 4;
        uint8_t     reserved0   : 1;                                                /* reserved bit */
        uint8_t     granularity : 1;                                                /* limit is in bytes (0) or pages (1) */
        uint8_t     size        : 1;                                                /* 16-bit (0) or 32-bit (1) */
        uint8_t     long_mode   : 1;                                                /* 64-bit (1) or 32/16-bits (0) */
        uint8_t     base_31_24;
        uint32_t    base_63_32;
        uint32_t    reserved;
    } __attribute__((packed));
};

/**
 * @brief This struct corresponds the GDTR, or Global Descriptor Table Register, which
 * is a special-purposed register describing the location and size of the GDT. The GDT
 * is to store the segment descriptors.
 * 
 * @note The `limit` should be inclusive end address of GDT;
 * @note We should load the GDTR with the `lgdt` instruction, and get current GDTR with
 * the `sgdt` instruction.
 */
struct gdtr {
    uint16_t limit;
    union gdt_entry *base;
} __attribute__((packed));

#define arch_load_gdtr(gdtr)            \
    asm volatile (                      \
        "lgdt %0\n"                     \
        :                               \
        : "m"(gdtr)                     \
        : "memory", "cc"                \
    )

#define IVT_INTERRUPT 0xE
#define IVT_TRAP      0xF

union idt_entry {
    uint64_t value[2];

    struct {
        uint16_t    offset_15_00;                                                   /* offset of the routine's entry relative to segment address */
        uint16_t    segment;                                                        /* segment selector in GDT or LDT */
        uint8_t     ist            : 3;                                             /* interrupt stack table index */
        uint8_t     reserved0      : 5;                                             /* reserved bits, must be 0 */
        uint8_t     type : 4;                                                       /* interrupt gate (0xE) or trap gate (0xF) */  
        uint8_t     zero : 1;                                                       /* reserved bit */
        uint8_t     dpl : 2;                                                        /* descriptor privilege level */
        uint8_t     present : 1;                                                    /* segment present in memory (1) or not (0) */
        uint16_t    offset_31_16;
        uint32_t    offset_63_32;
        uint32_t    reserved1;
    } __attribute__((packed));
};

/**
 * @brief This struct corresponds the IDTR, or Interrupt Descriptor Table Register, which
 * is a special-purposed register describing the location and size of the IDT. The IDT
 * is to store the interrupt descriptors.
 * 
 * @note We should load the IDTR with the `lidt` instruction, and get current IDTR with
 * the `sidt` instruction.
 */
struct idtr {
    uint16_t limit;
    union idt_entry *base;
} __attribute__((packed));
                                        
#define EXP_DIVIDE_ERROR                0x00                                        /* DIV or IDIV */
#define EXP_DEBUG                       0x01
#define EXP_NON_MASKABLE_INTERRUPT      0x02
#define EXP_BREAKPOINT                  0x03                                        /* INT3 */
#define EXP_OVERFLOW                    0x04                                        /* INTO */
#define EXP_BOUND_RANGE_EXCEEDED        0x05                                        /* BOUND */
#define EXP_INVALID_OPCODE              0x06                                        /* UD2 */
#define EXP_DEVICE_NOT_AVAILABLE        0x07
#define EXP_DOUBLE_FAULT                0x08
#define EXP_COPROCESSOR_SEGMENT_OVERRUN 0x09
#define EXP_INVALID_TSS                 0x0A
#define EXP_SEGMENT_NOT_PRESENT         0x0B
#define EXP_STACK_FAULT                 0x0C
#define EXP_GENERAL_PROTECTION          0x0D
#define EXP_PAGE_FAULT                  0x0E
#define EXP_RESERVED                    0x0F
#define EXP_X87_FLOATING_POINT          0x10
#define EXP_ALIGNMENT_CHECK             0x11
#define EXP_MACHINE_CHECK               0x12
#define EXP_SIMD_FLOATING_POINT         0x13
#define EXP_VIRTUALIZATION              0x14
#define EXP_CONTROL_PROTECTION          0x15
#define EXP_HYPERVISOR                  0x16
#define EXP_EXTERNAL_MIN                0x20

#define arch_load_idtr(idtr)            \
    asm volatile (                      \
        "lidt %0\n"                     \
        :                               \
        : "m"(idtr)                     \
        : "memory", "cc"                \
    )

union tss {
    uint64_t value[2];

    struct {
        uint32_t reserved0;
        uint64_t rsp0;                                                              /* stack pointer for ring 0 */
        uint64_t rsp1;                                                              /* stack pointer for ring 1 */
        uint64_t rsp2;                                                              /* stack pointer for ring 2 */
        uint64_t reserved1;
        uint64_t ist[7];                                                            /* stack pointer for interrupt */
        uint16_t reserved2;
        uint16_t io_map_base;                                                       /* offset of I/O map base address */
    } __attribute__((packed));
};

#endif
