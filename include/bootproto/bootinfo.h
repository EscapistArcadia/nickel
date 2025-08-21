#ifndef __NICKEL_BOOT_H__
#define __NICKEL_BOOT_H__

#include <stdint.h>                                                                 /* TODO: remove dependency, as you cannot suppose that */

#define NICKEL_BOOT_MAGIC                       0x4573636170697374                  /* "Escapist" in ASCII */
#define NICKEL_VERSION                          0xDEADBEEFECEBCAFE                  /* placeholder */
#define NICKEL_HEADER_OFFSET                    0xECEB                              /* offset of the header in the kernel binary */

/**
 * @brief Boot header structure contained in the kernel binary.
 */
struct nickel_boot_header {
    uint64_t magic;                                                                 /* magic number to verify, must equal to `NICKEL_BOOT_MAGIC` */
    uint64_t kernel_version;
    uint64_t kernel_size;
    uint64_t kernel_entry;                                                          /* the location the bootloader should jump to */
} __attribute__((packed));

/**
 * @brief Boot information structure passed to the kernel. This contains the header and
 *        some additional information.
 */
struct nickel_boot_info {
    struct nickel_boot_header header;
    
    uint64_t base_address;                                                          /* the base address of the kernel in memory */
    // uint64_t boot_type;

    uint64_t acpi_rsdp;
};

#endif
