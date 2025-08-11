#include <arch/apic/ipi.h>
#include <arch/apic/registers.h>

/**
 * @brief Writes a value to the interrupt command register of the APIC.
 * 
 * @param cmd The command struct to write.
 * @note The act of writing to the low doubleword of the ICR causes the IPI to be sent.
 */
#define apic_write_intr_cmd(cmd) \
    apic_write_reg(APIC_REG_INTR_CMD_HIGH, cmd.high); \
    apic_write_reg(APIC_REG_INTR_CMD_LOW, cmd.low);


