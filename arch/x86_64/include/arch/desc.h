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

#define SDA_ACCESSED            0x01
#define SDA_CODE_EXECONLY       0x00
#define SDA_CODE_READABLE       0x02
#define SDA_DATA_READONLY       0x00
#define SDA_DATA_WRITABLE       0x02
#define SDA_CODE_EQDPLONLY      0x00
#define SDA_CODE_CONFORMING     0x04
#define SDA_DATA_EXPAND_DOWN    0x00
#define SDA_DATA_GROWS_UP       0x04
#define SDA_CODE_SEGMENT        0x08
#define SDA_DATA_SEGMENT        0x00
// #define SDA_SYSTEM_SEGMENT      0x10
// #define SDA_PRESENT             0x80

#define SDAT_TSS16_AVAILABLE    0x01
#define SDAT_LDT                0x02
#define SDAT_TSS16_BUSY         0x03
#define SDAT_TSS32_AVAILABLE    0x09
#define SDAT_TSS32_BUSY         0x0B
#define SDAT_TSS64_AVAILABLE    0x09
#define SDAT_TSS64_BUSY         0x0B

#define SDF_LONG_MODE           0x2
#define SDF_SIZE_32BIT          0x4
#define SDF_GRANULARITY         0x8

/**
 * @brief This struct corresponds the GDT entry in x86_64, which describes a segment of
 * memory. Visit https://wiki.osdev.org/Global_Descriptor_Table or Intel Manual for more
 * information.
 */
typedef union gdt_entry {
    uint32_t value[4];

    struct {
        uint16_t    limit_15_00;                                                    /* number of accessible bytes from base */
        uint16_t    base_15_00;                                                     /* starting physical address of the segment */
        uint8_t     base_23_16;
        union {
            uint8_t     access_byte : 4;
            struct {
                uint8_t accessed : 1;
                uint8_t read_write : 1;                                             /* code: readable (1) or executable only(0); data: writable (1) or readonly (0) */
                uint8_t dir_cfm : 1;                                                /* code: executable if CPL == DPL (1) or CPL >= DPL (0); data: grows up (1) or down (0) */
                uint8_t seg_type : 1;                                               /* code segment (1) or data segment (0) */
            } __attribute__((packed));
            uint8_t     type : 4;                                                   /* segment type */
        };
        uint8_t not_sys_seg : 1;                                                    /* system segment (1) or code/data segment (0) */
        uint8_t dpl : 2;                                                            /* descriptor privilege level */
        uint8_t present : 1;                                                        /* segment present in memory (1) or not (0) */
        uint8_t limit_19_16 : 4;
        union {
            uint8_t     flag : 4;                                                   /* segment flag */
            struct {
                uint8_t reserved0 : 1;                                              /* reserved bit */
                uint8_t granularity : 1;                                            /* limit is in bytes (0) or pages (1) */
                uint8_t size : 1;                                                   /* 16-bit (0) or 32-bit (1) */
                uint8_t long_mode : 1;                                              /* reserved bits */
            } __attribute__((packed));
        };
        uint8_t  base_31_24;
        uint32_t base_63_32;
        uint32_t reserved;
    } __attribute__((packed));
} gdt_entry_t;

/**
 * @brief This struct corresponds the GDTR, or Global Descriptor Table Register, which
 * is a special-purposed register describing the location and size of the GDT. The GDT
 * is to store the segment descriptors.
 * 
 * @note The `limit` should be inclusive end address of GDT;
 * @note We should load the GDTR with the `lgdt` instruction, and get current GDTR with
 * the `sgdt` instruction.
 */
typedef struct gdtr {
    uint16_t limit;
    union gdt_entry *base;
} __attribute__((packed)) gdtr_t;

typedef union idt_entry {

} idt_entry_t;

/**
 * @brief This struct corresponds the IDTR, or Interrupt Descriptor Table Register, which
 * is a special-purposed register describing the location and size of the IDT. The IDT
 * is to store the interrupt descriptors.
 * 
 * @note We should load the IDTR with the `lidt` instruction, and get current IDTR with
 * the `sidt` instruction.
 */
typedef struct idtr {
    uint16_t limit;
    union idt_entry *base;
} __attribute__((packed)) idtr_t;

typedef union tss_entry {

} tss_entry_t;

#endif
