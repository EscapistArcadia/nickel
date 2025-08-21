#include <efi.h>
#include <efilib.h>

#include <bootinfo.h>

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
        .header = *header,
        .base_address = kernel_addr,
        // .acpi_rsdp = boot_info.acpi_xsdp
    };

    EFI_GUID gEfiAcpi20TableGuid = ACPI_20_TABLE_GUID;                              /* ACPI 2.0 table GUID */
    for (UINTN i = 0; i < SystemTable->NumberOfTableEntries; i++) {
        if (CompareGuid(&SystemTable->ConfigurationTable[i].VendorGuid, &gEfiAcpi20TableGuid) == 0) {
            boot_info.acpi_rsdp = (UINT64)SystemTable->ConfigurationTable[i].VendorTable; /* gets the ACPI XSDT address */
            // Print(L"ACPI XSDP Address: 0x%lx\n", boot_info.acpi_xsdp);
            break;                                                                  /* found the ACPI table */
        }
    }

    // Print(L"Boot Info: 0x%lx\n", &boot_info);
    // Print(L"  Kernel Base Address: 0x%lx\n", boot_info.base_address);
    // Print(L"  ACPI RSDP Address: 0x%lx\n", boot_info.acpi_rsdp);

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