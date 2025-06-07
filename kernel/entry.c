#include <limits.h>
#include <stdint.h>
// #include <arch/desc.h>

// volatile union gdt_entry entry[] = {
//     {{0}},
//     {   /* Kernel Code Segment */
//         .limit_15_00 = 0xFFFF,
//         .base_15_00 = 0x0000,
//         .base_23_16 = 0x00,
//         .access_mode = 0xA,
//         .not_sys_seg = 1,
//         .dpl = 0,
//         .present = 1,
//         .limit_19_16 = 0xF,
//         .granularity = 1,
//         .size = 1,
//         .long_mode = 1,
//         .base_31_24 = 0x00,
//         .base_63_32 = 0x00,
//         .reserved = 0
//     },
//     {   /* Kernel Data Segment */
//         .limit_15_00 = 0xFFFF,
//         .base_15_00 = 0x0000,
//         .base_23_16 = 0x00,
//         .access_mode = 0x2,
//         .not_sys_seg = 1,
//         .dpl = 0,
//         .present = 1,
//         .limit_19_16 = 0xF,
//         .granularity = 1,
//         .size = 1,
//         .long_mode = 1,
//         .base_31_24 = 0x00,
//         .base_63_32 = 0x00,
//         .reserved = 0
//     },
//     {   /* User Code Segment */
//         .limit_15_00 = 0xFFFF,
//         .base_15_00 = 0x0000,
//         .base_23_16 = 0x00,
//         .access_mode = 0xA,
//         .not_sys_seg = 1,
//         .dpl = 3,
//         .present = 1,
//         .limit_19_16 = 0xF,
//         .granularity = 1,
//         .size = 1,
//         .long_mode = 1,
//         .base_31_24 = 0x00,
//         .base_63_32 = 0x00,
//         .reserved = 0
//     },
//     {   /* User Data Segment */
//         .limit_15_00 = 0xFFFF,
//         .base_15_00 = 0x0000,
//         .base_23_16 = 0x00,
//         .access_mode = 0x2,
//         .not_sys_seg = 1,
//         .dpl = 3,
//         .present = 1,
//         .limit_19_16 = 0xF,
//         .granularity = 1,
//         .size = 1,
//         .long_mode = 1,
//         .base_31_24 = 0x00,
//         .base_63_32 = 0x00,
//         .reserved = 0
//     },
// };

// struct gdtr gdt = {
//     .limit = sizeof(entry) - 1,
//     .base = (union gdt_entry *)entry
// };

/**
 * @brief This function is the main entry point for the kernel. It is called from the bootloader
 *        after the CPU has been set up. It will diverge into different echosystems based on the
 *        architecture and the bootloader used, and eventually return back to the general
 *        kernel entry point for execution.
 * 
 * @param param the pointer to the parameter passed from the bootloader.
 */
static void NickelMain(void *param) {
    uint64_t i;
    for (i = 0; i < UINT_MAX; i++) {
        asm volatile (
            "nop\n"
            : 
            : 
            : "memory", "cc"
        );
    }
}