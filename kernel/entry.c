#include <limits.h>
#include <stdint.h>

#include <bootproto/bootinfo.h>
#include <acpi.h>
#include <arch/flat_gdt.h>
#include <arch/default_idt.h>

#if defined(NICKEL_X86_64)
static void arch_test(void) {
    extern uint8_t __ld_data_start, __ld_data_end, __ld_data_lma;
    uint8_t *src = &__ld_data_lma, *dst = &__ld_data_start;
    while (dst < &__ld_data_end) {
        *dst++ = *src++;
    }

    asm volatile (
        "lgdt %0\n"
        :
        : "m"(gdtr)
        : "memory"
    );

    struct gdtr gdt_ptr;
    asm volatile (
        "sgdt %0\n"
        : "=m"(gdt_ptr)
        :
        : "memory"
    );

    asm volatile (
        "ends_up:\n"
        "movw $0x20, %%ax\n"
        "movw %%ax, %%ds\n"
        "movw %%ax, %%es\n"
        "movw %%ax, %%fs\n"
        "movw %%ax, %%gs\n"
        "movw %%ax, %%ss\n"
        // "ljmp $0x10, halt\n"
        :
        :
        : "memory", "ax"
    );

    init_idt();
    asm volatile (
        "lidt %0\n"
        "sti\n"
        :
        : "m"(idtr)
        : "memory"
    );

    struct idtr idt_ptr;
    asm volatile (
        "sidt %0\n"
        : "=m"(idt_ptr)
        :
        : "memory"
    );

    asm volatile (
        "ud2\n"  // Trigger an invalid opcode exception
    );
}
#elif defined(NICKEL_AARCH64)
#elif defined(NICKEL_RISCV64)
#endif

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
        goto halt;  /* halt the CPU if the magic number is incorrect */
    } else if (boot_info->header.kernel_version != NICKEL_VERSION) {
        goto halt;  /* halt the CPU if the kernel version is incorrect */
    }

    arch_test();

    ret = acpi_init((struct acpi_xsdp_desc *)boot_info->acpi_rsdp);
    if (ret < 0) {
        goto halt;  /* halt the CPU if ACPI initialization fails */
    }
halt:
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
