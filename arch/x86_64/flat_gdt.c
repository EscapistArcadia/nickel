#include <arch/flat_gdt.h>

union gdt_entry gdt_entries[] = {
    {{0}},
    {   /* Kernel Code Segment */
        .limit_15_00 = 0xFFFF,
        .base_15_00 = 0x0000,
        .base_23_16 = 0x00,
        .access_mode = 0xA,
        .not_sys_seg = 1,
        .dpl = 0,
        .present = 1,
        .limit_19_16 = 0xF,
        .granularity = 1,
        .size = 1,
        .long_mode = 1,
        .base_31_24 = 0x00,
        .base_63_32 = 0x00,
        .reserved = 0
    },
    {   /* Kernel Data Segment */
        .limit_15_00 = 0xFFFF,
        .base_15_00 = 0x0000,
        .base_23_16 = 0x00,
        .access_mode = 0x2,
        .not_sys_seg = 1,
        .dpl = 0,
        .present = 1,
        .limit_19_16 = 0xF,
        .granularity = 1,
        .size = 1,
        .long_mode = 0,
        .base_31_24 = 0x00,
        .base_63_32 = 0x00,
        .reserved = 0
    },
    {   /* User Code Segment */
        .limit_15_00 = 0xFFFF,
        .base_15_00 = 0x0000,
        .base_23_16 = 0x00,
        .access_mode = 0xA,
        .not_sys_seg = 1,
        .dpl = 3,
        .present = 1,
        .limit_19_16 = 0xF,
        .granularity = 1,
        .size = 1,
        .long_mode = 1,
        .base_31_24 = 0x00,
        .base_63_32 = 0x00,
        .reserved = 0
    },
    {   /* User Data Segment */
        .limit_15_00 = 0xFFFF,
        .base_15_00 = 0x0000,
        .base_23_16 = 0x00,
        .access_mode = 0x2,
        .not_sys_seg = 1,
        .dpl = 3,
        .present = 1,
        .limit_19_16 = 0xF,
        .granularity = 1,
        .size = 1,
        .long_mode = 0,
        .base_31_24 = 0x00,
        .base_63_32 = 0x00,
        .reserved = 0
    },
};

struct gdtr gdtr = {
    .limit = sizeof(gdt_entries) - 1,
    .base = (union gdt_entry *)gdt_entries
};
