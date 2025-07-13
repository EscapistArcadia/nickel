#ifndef __NICKEL_BOOT_H__
#define __NICKEL_BOOT_H__

#include <stdint.h>                                                                 /* TODO: remove dependency, as you cannot suppose that */

/**
 * @brief Boot header structure contained in the kernel binary.
 */
struct nickel_boot_header {
    uint64_t magic;                                                                 /* magic number to verify, must equal to NICKEL_BOOT_MAGIC */
    uint64_t kernel_version;
    uint64_t kernel_size;
    uint64_t kernel_entry;                                                          /* the location the bootloader should jump to */
} __attribute__((packed));

struct nickel_boot_info {
    struct nickel_boot_header header;
};

#endif
