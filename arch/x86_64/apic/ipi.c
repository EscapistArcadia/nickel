#include <arch/apic/ipi.h>
#include <arch/apic/registers.h>

extern uint64_t apic_base;

/**
 * @brief Sends an INIT Inter-Processor Interrupt (IPI) to the specified APIC ID. An INIT IPI
 *        is used to reset a remote CPU to initial halting state.
 * 
 * @param apic_id The APIC ID of the target CPU, obtained from ACPI MADT table.
 * @todo  Older processor needs level-triggered INIT IPI with an explicit deassert. Check
 *        processor type and implement accordingly.
 */
inline void apic_send_init_ipi(uint8_t apic_id) {
    uint32_t ret;

    apic_write_reg(apic_base, APIC_REG_INTERRUPT_COMMAND_HIGH, uint32_t,
        APIC_DESTINATION_DWORD(apic_id)
    );

    apic_write_reg(apic_base, APIC_REG_INTERRUPT_COMMAND_LOW, uint32_t,             /* Write behavior of ICR LOW triggers delivery of the IPI. */
        APIC_DESTINATION_SHORTHAND_NONE
        | APIC_TRIGGER_MODE_EDGE
        | APIC_LEVEL_ASSERT
        // | APIC_DELIVERY_STATUS_IDLE                                              /* READONLY!!! */
        | APIC_DESTINATION_MODE_PHYSICAL
        | APIC_DELIVERY_MODE_INIT
        // | APIC_VECTOR(0)                                                         /* not used, reserved */
    );

    do {
        apic_read_reg(apic_base, APIC_REG_INTERRUPT_COMMAND_LOW, uint32_t, ret);
    } while (ret & APIC_DELIVERY_STATUS_PENDING);                                   /* Waits for IPI delivery to complete */
}

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
inline void apic_send_startup_ipi(uint8_t apic_id, uint8_t *startup_ip) {
    uint32_t ret, i, j;

    for (i = 0; i < 2; ++i) {
        apic_write_reg(apic_base, APIC_REG_INTERRUPT_COMMAND_HIGH, uint32_t,
            APIC_DESTINATION_DWORD(apic_id)
        );

        apic_write_reg(apic_base, APIC_REG_INTERRUPT_COMMAND_LOW, uint32_t,
            APIC_DESTINATION_SHORTHAND_NONE
            | APIC_TRIGGER_MODE_EDGE
            | APIC_LEVEL_ASSERT
            | APIC_DESTINATION_MODE_PHYSICAL
            | APIC_DELIVERY_MODE_STARTUP
            | APIC_VECTOR((uint64_t)startup_ip >> 12)
        );

        do {                                                                        /* TODO: verify if we need for SIPI */
            apic_read_reg(apic_base, APIC_REG_INTERRUPT_COMMAND_LOW, uint32_t, ret);
        } while (ret & APIC_DELIVERY_STATUS_PENDING);

        for (j = 0; j < 100000000; ++j) {
            __asm__ volatile("pause");
        }
    }
}
