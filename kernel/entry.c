#include <limits.h>
#include <stdint.h>

#include <bootproto/bootinfo.h>
#include <acpi.h>
/**
 * @brief This function is the main entry point for the kernel. It is called from the bootloader
 *        after the CPU has been set up. It will diverge into different echosystems based on the
 *        architecture and the bootloader used, and eventually return back to the general
 *        kernel entry point for execution.
 * 
 * @param param the pointer to the parameter passed from the bootloader.
 */
__attribute__((noreturn))
static void NickelMain(struct nickel_boot_info *boot_info) {
    int32_t ret;
    
    if (boot_info->header.magic != NICKEL_BOOT_MAGIC) {
        goto hlt;  /* halt the CPU if the magic number is incorrect */
    } else if (boot_info->header.kernel_version != NICKEL_VERSION) {
        goto hlt;  /* halt the CPU if the kernel version is incorrect */
    }

    ret = acpi_init((struct acpi_xsdp_desc *)boot_info->acpi_rsdp);
    if (ret < 0) {
        goto hlt;  /* halt the CPU if ACPI initialization fails */
    }

hlt:
    while (1);
}

/**
 * @brief This is the boot header that will be used by the bootloader to verify the kernel.
 * @note The `__attribute__((used))` is used to ensure that the linker does not remove this
 *       symbol due to optimization.
 */
__attribute__((used, section(".boot_header")))
static struct nickel_boot_header header = {
    .magic = NICKEL_BOOT_MAGIC,
    .kernel_version = NICKEL_VERSION,
    .kernel_size = 0,
    .kernel_entry = (uint64_t)NickelMain
};
