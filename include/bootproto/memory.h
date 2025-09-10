#ifndef __NICKEL_BOOTPROTO_MEMORY_H__
#define __NICKEL_BOOTPROTO_MEMORY_H__

#include <stdint.h>                                                                 /* TODO: remove dependency, as you cannot suppose that */

// #ifdef NICKEL_BOOT_UEFI                                                          /* TODO: support BIOS MBR boot, and let compiler ignore this */
#define NICKEL_UEFI_MEM_RESERVED                0
#define NICKEL_UEFI_MEM_LOADER_CODE             1
#define NICKEL_UEFI_MEM_LOADER_DATA             2
#define NICKEL_UEFI_MEM_BOOT_SERVICES_CODE      3
#define NICKEL_UEFI_MEM_BOOT_SERVICES_DATA      4
#define NICKEL_UEFI_MEM_RUNTIME_SERVICES_CODE   5
#define NICKEL_UEFI_MEM_RUNTIME_SERVICES_DATA   6
#define NICKEL_UEFI_MEM_CONVENTIONAL            7
#define NICKEL_UEFI_MEM_UNUSABLE                8
#define NICKEL_UEFI_MEM_ACPI                    9
#define NICKEL_UEFI_MEM_FIRMWARE                10
#define NICKEL_UEFI_MEM_MMIO                    11
#define NICKEL_UEFI_MEM_MMIO_PORTSPACE          12
#define NICKEL_UEFI_MEM_PALCODE                 13
#define NICKEL_UEFI_MEM_PERSISTENT              14
#define NICKEL_UEFI_MEM_UNACCEPTED              15
#define NICKEL_UEFI_MEM_MAX                     16

#define NICKEL_UEFI_MEM_ATTR_RUNTIME            0x8000000000000000

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
