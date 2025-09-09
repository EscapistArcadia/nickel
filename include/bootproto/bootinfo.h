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
    
    struct {
        uint64_t key;
        uint64_t mmap;
        uint64_t map_size;
        uint64_t desc_size;
        uint32_t desc_version;
    } efi_mmap;
};

// #ifdef NICKEL_BOOT_UEFI                                                          /* TODO: support BIOS MBR boot, and let compiler ignore this */
#define NICKEL_UEFI_MEM_RESERVED                0
/* memory with type 1 - 6 are for UEFI code/data, but they could be discarded after we enter the kernel */
/* memory with type 7 is free memory */
#define NICKEL_UEFI_MEM_UNUSABLE                8
#define NICKEL_UEFI_MEM_ACPI                    9
#define NICKEL_UEFI_MEM_FIRMWARE                10
#define NICKEL_UEFI_MEM_MMIO                    11
#define NICKEL_UEFI_MEM_MMIO_PORTSPACE          12
#define NICKEL_UEFI_MEM_PALCODE                 13
#define NICKEL_UEFI_MEM_PERSISTENT              14
#define NICKEL_UEFI_MEM_UNACCEPTED              15
#define NICKEL_UEFI_MEM_MAX                     16

/**
 * @brief Equivalent structure of `EFI_MEMORY_DESCRIPTOR` in UEFI specification. Since we
 *        cannot ensure the availability of UEFI memory map, we parse the memory map after
 *        entering the kernel.
 */
struct efi_memory_desc {
    uint32_t type;
    uint32_t pad;
    uint64_t physical_start;
    uint64_t virtual_start;
    uint64_t number_of_pages;
    uint64_t attribute;
};
// #endif

#endif
