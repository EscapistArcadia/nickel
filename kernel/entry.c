#include <limits.h>
#include <stdint.h>

#include <bootproto/bootinfo.h>
#include <acpi.h>
#include <arch/flat_gdt.h>
#include <arch/default_idt.h>

#include <arch/apic/registers.h>
#include <arch/apic/ipi.h>

#if defined(NICKEL_X86_64)
volatile uint64_t apic_base;

static void apic_test(void) {
    volatile union apic_base_msr base_msr;
    volatile uint32_t eax, ebx, ecx, edx;

    asm volatile (
        "cpuid\n"
        : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
        : "a"(0)
    );

    asm volatile (
        "cpuid\n"
        : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
        : "a"(1)
    );

    apic_read_base_msr(base_msr);
    if (!base_msr.apic_enable) {
        base_msr.apic_enable = 1;  /* enable the APIC */
        apic_write_base_msr(base_msr);
    }

    apic_base = base_msr.apic_base << 12;

    extern volatile uint8_t ap_startup[], ap_startup_end[];

    // for debug
    static volatile uint8_t *ap_startup_code = (uint8_t *)0x8000;
    *ap_startup_code = 0xEB;

    for (volatile uint8_t *src = ap_startup, *dst = ap_startup_code; src < ap_startup_end; src++, dst++) {
        *dst = *src;
    }

// extern volatile uint32_t cores = 0, enabled_cores = 0;
    extern volatile struct acpi_processor_local_apic processors[256];

    apic_send_init_ipi(processors[1].apic_id);
    
    for (volatile int i = 0; i < 20000000; i++) {
        __asm__ volatile("pause");
    }
    
    apic_send_startup_ipi(processors[1].apic_id, (uint8_t *)ap_startup_code);

    while (1);
}

static void arch_test(void) {
    /**
     * @note The following snippet of code performs data section initialization (LMA). `__ld_data_start`, `__ld_data_end`,
     * and `__ld_data_lma` are linker-defined symbols. During the compilation, initialized data is placed at the LMA
     * (Load Memory Address), but at runtime, we need to copy it to the VMA (Virtual Memory Address) for execution.
     * This is necessary because the kernel may be loaded at a different address than where it was compiled.
     * 
     * @todo This is a temporary solution. I remembered that I don't need to manually do this. I will find out why.
     */
    // extern uint8_t __ld_data_start, __ld_data_end, __ld_data_lma;
    // uint8_t *src = &__ld_data_lma, *dst = &__ld_data_start;
    // while (dst < &__ld_data_end) {
    //     *dst++ = *src++;
    // }

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
        "pushq $0x10\n"
        "pushq $activate_cs\n"                                                      /* code segment selector with intended place to jump */
        "lretq\n"                                                                   /* to activate the new code segment */
        "activate_cs:\n"                                                            /* ljmp in long mode is a little bit complex, so we use lret */
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

    // asm volatile (
    //     "ud2\n"                                                                     /* triggers invalid opcode exception */
    //     "int3\n"                                                                    /* triggers breakpoint exception */
    // );

    volatile union pml_entry *pml4e;
    asm volatile (
        "movq %%cr3, %0\n"
        : "=r"(pml4e)
        :
        : "memory"
    );

    pml4e = (union pml_entry *)((uint64_t)pml4e & 0xFFFFFFFFFFFFF000);

    apic_test();
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

    ret = acpi_init((struct acpi_xsdp_desc *)boot_info->acpi_rsdp);
    if (ret < 0) {
        goto halt;  /* halt the CPU if ACPI initialization fails */
    }

    arch_test();
halt:
    while (1);
}

/**
 * @brief This is the boot header that will be used by the bootloader to verify the kernel.
 * @note The `__attribute__((used))` is used to ensure that the linker does not remove this
 *       symbol due to optimization.
 */
__attribute__((used, section(".boot_header"), aligned(1)))
static struct nickel_boot_header header = {
    .magic = NICKEL_BOOT_MAGIC,
    .kernel_version = NICKEL_VERSION,
    .kernel_size = 0,
    .kernel_entry = (uint64_t)NickelMain
};
