#include <limits.h>
#include <arch/desc.h>

volatile union gdt_entry entry[] = {
    {{0}},
    {   /* Kernel Code Segment */
        .limit_15_00 = 0xFFFF,
        .base_15_00 = 0x0000,
        .base_23_16 = 0x00,
        .access_byte = SDA_CODE_SEGMENT | SDA_CODE_READABLE,
        .not_sys_seg = 1,
        .dpl = 0,
        .present = 1,
        .limit_19_16 = 0xF,
        .flag = SDF_GRANULARITY | SDF_LONG_MODE,
        .base_31_24 = 0x00
    },
    {   /* Kernel Data Segment */
        .limit_15_00 = 0xFFFF,
        .base_15_00 = 0x0000,
        .base_23_16 = 0x00,
        .access_byte = SDA_DATA_SEGMENT | SDA_DATA_WRITABLE,
        .not_sys_seg = 1,
        .dpl = 0,
        .present = 1,
        .limit_19_16 = 0xF,
        .flag = SDF_GRANULARITY | SDF_LONG_MODE,
        .base_31_24 = 0x00
    },
    {   /* User Code Segment */
        .limit_15_00 = 0xFFFF,
        .base_15_00 = 0x0000,
        .base_23_16 = 0x00,
        .access_byte = SDA_CODE_SEGMENT | SDA_CODE_READABLE,
        .not_sys_seg = 1,
        .dpl = 3,
        .present = 1,
        .limit_19_16 = 0xF,
        .flag = SDF_GRANULARITY | SDF_LONG_MODE,
        .base_31_24 = 0x00
    },
    {   /* User Data Segment */
        .limit_15_00 = 0xFFFF,
        .base_15_00 = 0x0000,
        .base_23_16 = 0x00,
        .access_byte = SDA_DATA_SEGMENT | SDA_DATA_WRITABLE,
        .not_sys_seg = 1,
        .dpl = 3,
        .present = 1,
        .limit_19_16 = 0xF,
        .flag = SDF_GRANULARITY | SDF_LONG_MODE,
        .base_31_24 = 0x00
    }
};

struct gdtr gdt = {
    .limit = sizeof(entry) - 1,
    .base = (union gdt_entry *)entry
};

static void NickelMain(void) {
    __UINT64_TYPE__ i;
    volatile struct gdtr *g;
    for (i = 0; i < UINT_MAX; i++) {
        if (i % 100000000 == 0) {
            __asm__ volatile (
                "lgdt %0\n"
                // "sgdt %1\n"
                :
                : "m"(gdt)
                //   "m"((uint64_t)g)
                : "memory", "cc"
            );
        }
    }
}