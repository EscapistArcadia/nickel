#ifndef __NICKEL_X86_64_APIC_IPI__
#define __NICKEL_X86_64_APIC_IPI__

#include <stdint.h>

/**
 * @brief Sends an INIT Inter-Processor Interrupt (IPI) to the specified APIC ID. An INIT IPI
 *        is used to reset a remote CPU to initial halting state.
 * 
 * @param apic_id The APIC ID of the target CPU, obtained from ACPI MADT table.
 * @todo  Older processor needs level-triggered INIT IPI with an explicit deassert. Check
 *        processor type and implement accordingly.
 */
extern void apic_send_init_ipi(uint8_t apic_id);

/**
 * @brief Sends a Startup Inter-Processor Interrupt (IPI) to the specified APIC ID. An Startup
 *        IPI is used to start a remote CPU at a specific page-aligned address from a halted
 *        state.
 *
 * @param apic_id The APIC ID of the target CPU.
 * @param startup_ip The startup instruction pointer for the target CPU.
 * 
 * @note The `startup_ip` should be a **page-aligned** address, and the CPU will start executing
 *       from this address after receiving the IPI.
 */
extern void apic_send_startup_ipi(uint8_t apic_id, uint8_t *startup_ip);

#endif
