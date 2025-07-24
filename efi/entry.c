#include <efi.h>
#include <efilib.h>

/**
 * @brief Checks if returned status is equal to the supposed status.
 * 
 * @param status The returned status
 * @param target The supposed status
 */
#define EFI_CHECK_STATUS(status, target)            \
if (status != target) {                             \
    Print(L"[%d] Bad status: %r; Intended status: %r", __LINE__, status, target);\
    while (1);                                      \
}

/* the name of kernel executable in file system */
#define KERNEL_FILE_NAME L"nickel.exe"

/**
 * @brief Get amount of 4KB pages by size of executable
 * 
 * @param size The size of executable
 * @return The amount of 4KB pages
 */
#define KERNEL_PAGE_COUNT(size) (((size) / EFI_PAGE_SIZE) + 1)

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
                                        
#define ACPI_RSDP_SIGNATURE                     "RSD PTR "
#define ACPI_MADT_SIGNATURE                     "APIC"

#define ACPI_MADT_APIC_TYPE_PROCESSOR           0
#define ACPI_MADT_APIC_TYPE_IO                  1
#define ACPI_MADT_APIC_TYPE_INTERRUPT           2
#define ACPI_MADT_APIC_TYPE_NMI_SOURCE          3
#define ACPI_MADT_APIC_TYPE_LOCAL_NMI           4
#define ACPI_MADT_APIC_TYPE_LOCAL_ADDR_OVERRIDE 5
#define ACPI_MADT_APIC_TYPE_IO_SAPIC            6
#define ACPI_MADT_APIC_TYPE_LOCAL_SAPIC         7
#define ACPI_MADT_APIC_TYPE_PLATFORM_INTERRUPT  8
#define ACPI_MADT_APIC_TYPE_PROCESSOR_X2        9
#define ACPI_MADT_APIC_TYPE_LOCAL_X2_NMI        10
#define ACPI_MADT_APIC_TYPE_GIC_CPU_INTERFACE   11
// TODO

#define ACPI_PROCESSOR_LOCAL_ENABLED            0x01
#define ACPI_PROCESSOR_LOCAL_ONLINE_CABLE       0x02

#define ACPI_MPS_INTI_FLAG_POLARITY_MASK        0x03
#define ACPI_MPS_INTI_FLAG_POLARITY_CONFIRM     0x00
#define ACPI_MPS_INTI_FLAG_POLARITY_ACTIVE_HIGH 0x01
#define ACPI_MPS_INTI_FLAG_POLARITY_ACTIVE_LOW  0x03

#define ACPI_MPS_INTI_FLAG_TRIGGER_MASK         0x0C
#define ACPI_MPS_INTI_FLAG_TRIGGER_CONFIRM      0x00
#define ACPI_MPS_INTI_FLAG_TRIGGER_EDGE         0x04
#define ACPI_MPS_INTI_FLAG_TRIGGER_LEVEL        0x0C

/**
 * @brief This structure aligns with the RSDP (Root System Description Pointer) table as defined 
 *        in the ACPI specification. It is the root of the ACPI tables. Note that some fields are
 *        only present in the XSDP (Extended System Description Pointer), or 2.0.
 */
struct acpi_rsdp_desc {
    char signature[8];                                                              /* MUST BE "RSD PTR " */
    uint8_t rsdp_checksum;                                                          /* checksum of fields in the ACPI 1.0 (first 20 bytes) */
    char oem_id[6];
    uint8_t revision;                                                               /* 0 for ACPI 1.0, 2 for ACPI 2.0 */
    uint32_t rsdt_address;                                                          /* 32-bit RSDT address (for ACPI 1.0) */

    /* XSDP ONLY */
    uint32_t length;                                                                /* length of the XSDT in bytes */
    uint64_t xsdt_address;                                                          /* 64-bit XSDT address (for ACPI 2.0) */
    uint8_t xdsp_checksum;                                                          /* checksum of the XSDT (including rsdp_checksum) */
    char reserved[3];
} __attribute__((packed));

/**
 * @brief This structure aligns with the RSDT (Root System Description Table) as defined
 *        in the ACPI specification. It contains pointers to ACPI tables, for ACPI 1.0.
 */
struct acpi_rsdt_desc {
    char signature[4];                                                              /* MUST BE "RSDT" */
    uint32_t length;                                                                /* length of the RSDT in bytes */
    uint8_t revision;
    uint8_t checksum;
    char oem_id[6];                                                                 /* OEM ID */
    char oem_table_id[8];                                                           /* manufacture model ID, must match the OEM table ID */
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
    uint32_t entries[0];                                                            /* variable length entries */
} __attribute__((packed));

/**
 * @brief This structure aligns with the XSDT (Extended System Description Table) as defined
 *        in the ACPI specification. It is an extended version of the RSDT, which is supported
 *        in ACPI 2.0 and later. It contains pointers to ACPI tables. 
 */
struct acpi_xsdt_desc {
    char signature[4];                                                              /* MUST BE "XSDT" */
    uint32_t length;                                                                /* length of the XSDT in bytes */
    uint8_t revision;
    uint8_t checksum;
    char oem_id[6];                                                                 /* OEM ID */
    char oem_table_id[8];                                                           /* manufacture model ID, must match the OEM table ID */
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
    uint64_t entries[0];                                                            /* variable length entries */
} __attribute__((packed));

/**
 * @brief This structure aligns with the common header of all ACPI tables. In other words, all
 *        ACPI tables start with this header, followed by table-specific data.
 */
struct acpi_desc_header {
    char signature[4];
    uint32_t length;                                                                /* length of the table in bytes */
    uint8_t revision;
    uint8_t checksum;
    char oem_id[6];
    char oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
} __attribute__((packed));

struct acpi_intr_ctrl_desc {
    uint8_t type;
    uint8_t length;
    
    union {
        struct {
            uint8_t uid;
            uint8_t apic_id;
            uint32_t flags;
        } __attribute__((packed)) processor;
        struct {
            uint8_t apic_id;
            uint8_t reserved;
            uint32_t apic_addr;
            uint32_t global_interrupt_base;
        } __attribute__((packed)) io;
        struct {
            uint8_t bus;
            uint8_t source;
            uint32_t global_interrupt;
            uint16_t flags;
        } __attribute__((packed)) interrupt_override;
        struct {
            uint16_t flags;
            uint32_t global_interrupt;
        } __attribute__((packed)) nmi_source;
        struct {
            uint8_t uid;
            uint16_t flags;
            uint8_t local_interrupt;
        } __attribute__((packed)) local_apic_nmi;
        struct {
            uint16_t reserved;
            uint32_t interface;
            uint32_t uid;
            uint32_t flags;
            uint32_t parking_protocol_version;
            uint32_t perf_gsi;
            uint64_t parked_addr;
            uint64_t physical_addr;
            uint64_t gicv;
            uint64_t gich;
            uint32_t vgic_intr;
            uint64_t gcir_base;
            uint64_t mpidr;
            uint8_t power_eff_class;
            uint8_t reserved1;
            uint16_t spe_overflow_intr;
            uint16_t trbe_intr;
        } __attribute__((packed)) gic;
    };
} __attribute__((packed));

/**
 * @brief This structure aligns with the MADT (Multiple APIC Description Table) as defined
 *        in the ACPI specification. It contains information about the local APICs and I/O
 *        APICs in the system, which can be used for processor detection and management,
 *          as well as interrupt routing. Note that some fields are reserved.
 */
struct acpi_madt_desc {
    struct acpi_desc_header header;
    uint32_t local_apic_address;                                                   /* Local APIC address */
    uint32_t flags;
    // struct acpi_intr_ctrl_desc entries[0];                                      /* variable length entries, but not pointers */
} __attribute__((packed));

void acpi_init(EFI_SYSTEM_TABLE *SystemTable) {
    EFI_STATUS status;
    EFI_CONFIGURATION_TABLE *config_table = SystemTable->ConfigurationTable;
    UINTN i;

    static EFI_GUID gEfiAcpi20TableGuid = ACPI_20_TABLE_GUID;
    struct acpi_rsdp_desc *rsdp = NULL;
    struct acpi_xsdt_desc *xsdt = NULL;
    struct acpi_madt_desc *madt = NULL;

    for (i = 0; i < SystemTable->NumberOfTableEntries; i++) {
        if (CompareGuid(&config_table[i].VendorGuid, &gEfiAcpi20TableGuid) == 0) {
            rsdp = (struct acpi_rsdp_desc *)config_table[i].VendorTable;
            // Print(L"Found ACPI RSDP at: %p\n", rsdp);
            // Process RSDP here...
            break;
        }
    }

    if (i == SystemTable->NumberOfTableEntries) {
        Print(L"ACPI RSDP not found!\n");
    }

    struct acpi_rsdt_desc *rsdt = (struct acpi_rsdt_desc *)(uintptr_t)rsdp->rsdt_address;
    Print(L"ACPI RSDT found at: %p, %u\n", rsdt, sizeof(*rsdt));
    Print(L"RSDT Signature: %c%c%c%c\n", rsdt->signature[0], rsdt->signature[1], rsdt->signature[2], rsdt->signature[3]);
    Print(L"RSDT Length: %u\n", rsdt->length);
    

    xsdt = (struct acpi_xsdt_desc *)rsdp->xsdt_address;
    Print(L"ACPI XSDT found at: %p, %u\n", xsdt, sizeof(*xsdt));
    Print(L"XSDT Signature: %c%c%c%c\n", xsdt->signature[0], xsdt->signature[1], xsdt->signature[2], xsdt->signature[3]);
    Print(L"XSDT Length: %u\n", xsdt->length);
    // Print(L"XSDT Revision: %u\n", xsdt->revision);
    // Print(L"XSDT Checksum: %u\n", xsdt->checksum);
    // Print(L"XSDT OEM ID: %.6s\n", xsdt->oem_id);
    // Print(L"XSDT OEM Table ID: %.8s\n", xsdt->oem_table_id);
    // Print(L"XSDT OEM Revision: %u\n", xsdt->oem_revision);
    // Print(L"XSDT Creator ID: %u\n", xsdt->creator_id);
    // Print(L"XSDT Creator Revision: %u\n", xsdt->creator_revision);

    for (i = 0; i < (xsdt->length - sizeof(*xsdt)) / sizeof(uint64_t); i++) {
        Print(L"XSDT Entry %u: %p\n", i, xsdt->entries[i]);
        struct acpi_desc_header *entry = (struct acpi_desc_header *)(UINTN)xsdt->entries[i];
        Print(L"  Signature: %c%c%c%c\n", entry->signature[0], entry->signature[1], entry->signature[2], entry->signature[3]);
        if (strncmpa(entry->signature, ACPI_MADT_SIGNATURE, 4) == 0) {
            madt = (struct acpi_madt_desc *)entry;
            Print(L"Found ACPI MADT at: %p LOCAL INTERRUPT CONTROLLER ADDRESS: %lx, FLAGS: %lx\n", madt, madt->local_apic_address, madt->flags);
        }
    }

    Print(L"ACPI MADT found at: %p with length %u\n", madt, madt->header.length);

    const uint32_t *lapic_info = (uint32_t *)((uint64_t)madt->local_apic_address + 0xC0);
    Print(L"LAPIC ID: %lx, %lx\n", lapic_info, *lapic_info);

    for (i = 44; i < (madt->header.length);) {
        struct acpi_intr_ctrl_desc *entry = (struct acpi_intr_ctrl_desc *)((UINT8 *)madt + i);
        Print(L"MADT Entry %u: %p\n", i, (entry->type));
        if (entry->type == 0) {
            Print(L"  Local APIC: UID=%u, APIC_ID=%u, APIC_FLAGS=%lx, LENGTH=%u\n",
                  entry->processor.uid,
                  entry->processor.apic_id,
                  entry->processor.flags,
                  entry->length);
        } else if (entry->type == 1) {
            Print(L"  I/O APIC: UID=%u, APIC_ID=%u, GLOBAL INTERRUPT BASE=%u, LENGTH=%u\n",
                  entry->io.apic_id,
                  entry->io.apic_addr,
                  entry->io.global_interrupt_base,
                  entry->length);
        } else if (entry->type == 2) {
            Print(L"  Interrupt Source Override: BUS=%u, SOURCE=%u, GLOBAL INTERRUPT=%u, FLAGS=%u, LENGTH=%u\n",
                  entry->interrupt_override.bus,
                  entry->interrupt_override.source,
                  entry->interrupt_override.global_interrupt,
                  entry->interrupt_override.flags,
                  entry->length);
        } else if (entry->type == 3) {
            Print(L"  NMI Source: GLOBAL INTERRUPT=%u, FLAGS=%u, LENGTH=%u\n",
                  entry->nmi_source.global_interrupt,
                  entry->nmi_source.flags,
                  entry->length);
        } else if (entry->type == 4) {
            Print(L"  Local APIC NMI: UID=%u, FLAGS=%u, LOCAL INTERRUPT=%u, LENGTH=%u\n",
                  entry->local_apic_nmi.uid,
                  entry->local_apic_nmi.flags,
                  entry->local_apic_nmi.local_interrupt,
                  entry->length);
        } else if (entry->type == ACPI_MADT_APIC_TYPE_GIC_CPU_INTERFACE) {
            Print(L"  GIC CPU Interface: UID=%u, FLAGS=%u, PARKING PROTOCOL VERSION=%u, "
                  "PERF GSI=%u, PARKED ADDRESS=0x%lx, PHYSICAL ADDRESS=0x%lx, "
                  "GICV=0x%lx, GICH=0x%lx, VGIC_INTR=%u, GCIR_BASE=0x%lx, "
                  "MPIDR=0x%lx, POWER EFF CLASS=%u, SPE OVERFLOW INTR=%u, TRBE INTR=%u, LENGTH=%u, sizeof(entry) = %u %u\n",
                  entry->gic.uid,
                  entry->gic.flags,
                  entry->gic.parking_protocol_version,
                  entry->gic.perf_gsi,
                  entry->gic.parked_addr,
                  entry->gic.physical_addr,
                  entry->gic.gicv,
                  entry->gic.gich,
                  entry->gic.vgic_intr,
                  entry->gic.gcir_base,
                  entry->gic.mpidr,
                  entry->gic.power_eff_class,
                  entry->gic.spe_overflow_intr,
                  entry->gic.trbe_intr,
                  entry->length, sizeof(struct acpi_intr_ctrl_desc), sizeof(entry->gic));
        } else {
            Print(L"  Unknown MADT Entry Type: %u, LENGTH=%u\n", entry->type, entry->length);
        }
        i += entry->length;                                                     /* move to the next entry */
    }
}

/**
 * @brief The entry point of the UEFI bootloader. It is the first snippet of customized
 * code that is executed after the UEFI firmware has initialized the hardware. We just
 * find some device-related information, load kernel to the memory, and jump to it with
 * necessary information.
 * 
 * @param ImageHandle The image handle of the UEFI bootloader;
 * @param SystemTable The system table of the UEFI firmware.
 */
EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    EFI_STATUS status = EFI_SUCCESS;

    InitializeLib(ImageHandle, SystemTable);                                        /* must call this */
                                               
    /* **************************************************
     * *                   Preprocess                   *
     * ************************************************** */
    status = uefi_call_wrapper(SystemTable->BootServices->SetWatchdogTimer, 4, 0, 0, 0, NULL);
    EFI_CHECK_STATUS(status, EFI_SUCCESS);                                          /* disables the watchdog */

    status = uefi_call_wrapper(SystemTable->ConOut->ClearScreen, 1, SystemTable->ConOut);
    EFI_CHECK_STATUS(status, EFI_SUCCESS);                                          /* clears the existing brand icon */

    /* **************************************************
     * *         Get Memory Map and Descriptors         *
     * ************************************************** */
    EFI_MEMORY_DESCRIPTOR *memory_map = NULL;
    UINTN memory_map_size = 0, map_key = 0, valid_map_key = 0, descriptor_size = 0;
    UINT32 descriptor_version = 0;
    status = uefi_call_wrapper(SystemTable->BootServices->GetMemoryMap, 5,
                               &memory_map_size, memory_map, &map_key,
                               &descriptor_size, &descriptor_version);
    EFI_CHECK_STATUS(status, EFI_BUFFER_TOO_SMALL);                                 /* must get this, because the first call is for buffer size */
    memory_map_size += (4 * descriptor_size);
    
    status = uefi_call_wrapper(SystemTable->BootServices->AllocatePool, 3, 
                               EfiLoaderData, memory_map_size, (VOID **)&memory_map);
    EFI_CHECK_STATUS(status, EFI_SUCCESS);                                          /* allocates memory based on size returned*/

    status = uefi_call_wrapper(SystemTable->BootServices->GetMemoryMap, 5,
                               &memory_map_size, memory_map, &map_key,
                               &descriptor_size, &descriptor_version);
    EFI_CHECK_STATUS(status, EFI_SUCCESS);                                          /* gets the memory map */

    // for (UINTN i = 0; i < memory_map_size / descriptor_size; i++) {
    //     EFI_MEMORY_DESCRIPTOR *desc = (EFI_MEMORY_DESCRIPTOR *)((UINT8 *)memory_map + i * descriptor_size);
    //     Print(L"Memory Descriptor %d: Type: %u, PhysicalStart: 0x%lx, PhysicalEnd: 0x%lx, Attribute: 0x%lx\n",
    //           i, desc->Type, desc->PhysicalStart, desc->PhysicalStart + (desc->NumberOfPages * EFI_PAGE_SIZE), desc->Attribute);
    // }

    // while (1);

    acpi_init(SystemTable);

    /* **************************************************
     * *        Load Kernel Executable to Memory        *
     * ************************************************** */
    EFI_LOADED_IMAGE *fs_image = NULL;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *sfs = NULL;
    EFI_FILE_PROTOCOL *root = NULL, *kernel = NULL;
    EFI_FILE_INFO *file_info = NULL;
    UINTN file_info_size = 0;
    status = uefi_call_wrapper(SystemTable->BootServices->HandleProtocol, 3, 
                               ImageHandle, &LoadedImageProtocol, (VOID **)&fs_image);
    EFI_CHECK_STATUS(status, EFI_SUCCESS);

    status = uefi_call_wrapper(SystemTable->BootServices->HandleProtocol, 3, 
                               fs_image->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (VOID **)&sfs);
    EFI_CHECK_STATUS(status, EFI_SUCCESS);                                          /* obtains objects for the volume */

    status = uefi_call_wrapper(sfs->OpenVolume, 2, sfs, &root);
    EFI_CHECK_STATUS(status, EFI_SUCCESS);                                          /* file system protocol */

    status = uefi_call_wrapper(root->Open, 5,
                               root, &kernel, KERNEL_FILE_NAME, EFI_FILE_MODE_READ, 0);
    EFI_CHECK_STATUS(status, EFI_SUCCESS);                                          /* opens the kernel executable file */

    status = uefi_call_wrapper(kernel->GetInfo, 4,
                               kernel, &gEfiFileInfoGuid, &file_info_size, NULL);
    EFI_CHECK_STATUS(status, EFI_BUFFER_TOO_SMALL);

    status = uefi_call_wrapper(SystemTable->BootServices->AllocatePool, 3, 
                               EfiLoaderData, file_info_size, (VOID **)&file_info);
    EFI_CHECK_STATUS(status, EFI_SUCCESS);                                          /* obtains kernel executable file info */

    status = uefi_call_wrapper(kernel->GetInfo, 4,
                               kernel, &gEfiFileInfoGuid, &file_info_size, file_info);
    EFI_CHECK_STATUS(status, EFI_SUCCESS);

    EFI_PHYSICAL_ADDRESS kernel_addr = KERNEL_ADDRESS;
    status = uefi_call_wrapper(SystemTable->BootServices->AllocatePages, 4, 
                               AllocateAddress, EfiLoaderData, KERNEL_PAGE_COUNT(file_info->FileSize), &kernel_addr);
    EFI_CHECK_STATUS(status, EFI_SUCCESS);                                          /* allocates memory for kernel */

    status = uefi_call_wrapper(kernel->Read, 3, kernel, &file_info->FileSize, (VOID *)kernel_addr);
    EFI_CHECK_STATUS(status, EFI_SUCCESS);                                          /* reads kernel executable to memory */
    
    status = uefi_call_wrapper(kernel->Close, 1, kernel);
    EFI_CHECK_STATUS(status, EFI_SUCCESS);                                          /* closes the kernel executable file */

    status = uefi_call_wrapper(root->Close, 1, root);
    EFI_CHECK_STATUS(status, EFI_SUCCESS);                                          /* closes the root directory */

    struct nickel_boot_header *header = (struct nickel_boot_header *)(kernel_addr + NICKEL_HEADER_OFFSET);
    // Print(L"Kernel Magic: 0x%lx\n", header->magic);
    // Print(L"Kernel Version: 0x%lx\n", header->kernel_version);
    // Print(L"Kernel Size: 0x%lx\n", header->kernel_size);
    // Print(L"Kernel Entry: 0x%lx\n", header->kernel_entry);
    if (header->magic != NICKEL_BOOT_MAGIC || header->kernel_version != NICKEL_VERSION) {
        Print(L"Invalid kernel header!\n");
        while (1);                                                                  /* halt the CPU if the header is invalid */
    }

    struct nickel_boot_info boot_info = {
        .header = *header
    };

    /* **************************************************
     * *                Exit EFI Service                *
     * ************************************************** */
    status = uefi_call_wrapper(SystemTable->BootServices->GetMemoryMap, 5,
        &memory_map_size, memory_map, &map_key,
        &descriptor_size, &descriptor_version);
    // EFI_CHECK_STATUS(status, EFI_BUFFER_TOO_SMALL);                                 /* get real-time map key to exit boot service*/

    status = uefi_call_wrapper(SystemTable->BootServices->ExitBootServices, 2, 
                               ImageHandle, map_key);
    EFI_CHECK_STATUS(status, EFI_SUCCESS);                                          /* we can no longer call any UEFI routines */

    /* **************************************************
     * *                 Jump to Kernel                 *
     * ************************************************** */
    ((void (*)(struct nickel_boot_info *))header->kernel_entry)(&boot_info);        /* jumps to the kernel */

    while (1);                                                                      /* should not reach here */
    return status;
}