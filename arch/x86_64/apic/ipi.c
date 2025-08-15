#include <arch/apic/ipi.h>
#include <arch/apic/registers.h>

extern uint64_t apic_base;

/**
 * @brief Writes a value to the interrupt command register of the APIC.
 * 
 * @param cmd The command struct to write.
 * @note The act of writing to the low doubleword of the ICR causes the IPI to be sent.
 */
#define apic_write_intr_cmd(cmd) \
    apic_write_reg(apic_base, APIC_REG_INTERRUPT_COMMAND_HIGH, cmd.high, uint32_t); \
    apic_write_reg(apic_base, APIC_REG_INTERRUPT_COMMAND_LOW, cmd.low, uint32_t);

void apic_send_init_ipi(uint8_t apic_id) {
    volatile union apic_intr_cmd cmd;
    uint32_t ret;

    cmd.low = 0;
    cmd.vector = 0; /* not used, predefined */
    cmd.delivery_mode = APIC_DELIVERY_MODE_INIT;
    cmd.destination_mode = APIC_DESTINATION_MODE_PHYSICAL;
    // cmd.delivery_status = APIC_DELIVERY_STATUS_IDLE;
    cmd.level = APIC_LEVEL_ASSERT;
    cmd.trigger_mode = APIC_TRIGGER_MODE_EDGE;
    cmd.destination_shorthand = APIC_DESTINATION_SHORTHAND_NONE;
    cmd.destination = apic_id;
    // cmd.high = (apic_id << 24);

    apic_write_reg(apic_base, APIC_REG_INTERRUPT_COMMAND_HIGH, cmd.high, uint32_t);

    // do {
    //     apic_read_reg(apic_base, APIC_REG_INTERRUPT_COMMAND_LOW, ret, uint32_t);
    // } while (ret & 0xC000);

    // wait for some time
    // for (volatile int i = 0; i < 1000000; i++) {
    //     __asm__ volatile("pause");
    // }

    apic_write_reg(apic_base, APIC_REG_INTERRUPT_COMMAND_LOW, cmd.low, uint32_t);

    for (volatile int i = 0; i < 1000000; i++) {
        __asm__ volatile("pause");
    }

    do {
        apic_read_reg(apic_base, APIC_REG_INTERRUPT_COMMAND_LOW, ret, uint32_t);
    } while (ret & (1 << 12));

    // cmd.level = APIC_LEVEL_DEASSERT;
    // apic_write_intr_cmd(cmd);
}

void apic_send_startup_ipi(uint8_t apic_id, uint8_t *startup_rip) {
    volatile union apic_intr_cmd cmd;
    uint32_t ret;

    cmd.value = 0;
    cmd.vector = (uint8_t)((uint64_t)startup_rip >> 12);
    cmd.delivery_mode = APIC_DELIVERY_MODE_STARTUP;
    cmd.destination_mode = APIC_DESTINATION_MODE_PHYSICAL;
    // cmd.delivery_status = APIC_DELIVERY_STATUS_IDLE;
    cmd.level = APIC_LEVEL_ASSERT;
    cmd.trigger_mode = APIC_TRIGGER_MODE_EDGE;
    cmd.destination_shorthand = APIC_DESTINATION_SHORTHAND_NONE;
    cmd.destination = apic_id;

    apic_write_intr_cmd(cmd);

    for (volatile int i = 0; i < 1000000; i++) {
        __asm__ volatile("pause");
    }
    
    do {
        apic_read_reg(apic_base, APIC_REG_INTERRUPT_COMMAND_LOW, ret, uint32_t);
    } while (ret & (1 << 12));
}

// union apic_intr_cmd {
//     uint64_t value;

//     struct {
//         uint32_t low, high;
//     } __attribute__((packed));

//     struct {
//         uint8_t vector;
//         uint8_t delivery_mode : 3;
//         uint8_t destination_mode : 1;
//         uint8_t delivery_status : 1;
//         uint8_t reserved0 : 1;
//         uint8_t level : 1;
//         uint8_t trigger_mode : 1;
//         uint8_t reserved1 : 2;
//         uint8_t destination_shorthand : 2;
//         uint64_t reserved2 : 36;
//         uint8_t destination;
//     } __attribute__((packed));
// };

