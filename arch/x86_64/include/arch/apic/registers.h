#ifndef __NICKEL_X86_64_ACPI_REGISTERS__
#define __NICKEL_X86_64_ACPI_REGISTERS__

#include <stdint.h>

#define APIC_REG_ID                     0x0020ULL
#define APIC_REG_VERSION                0x0030ULL
#define APIC_REG_TASK_PRIORITY          0x0080ULL
#define APIC_REG_ARBITRATION_PRIORITY   0x0090ULL
#define APIC_REG_PROCESSOR_PRIORITY     0x00A0ULL
#define APIC_REG_EOI                    0x00B0ULL
#define APIC_REG_REMOTE_READ            0x00C0ULL
#define APIC_REG_LOGICAL_DESTINATION    0x00D0ULL
#define APIC_REG_DESTINATION_FORMAT     0x00E0ULL
#define APIC_REG_SPURIOUS_VECTOR        0x00F0ULL
#define APIC_REG_INSERVICE_VECTOR       0x0100ULL
#define APIC_REG_TRIGGER_MODE           0x0180ULL
#define APIC_REG_INTERRUPT_REQUEST      0x0200ULL
#define APIC_REG_ERROR_STATUS           0x0280ULL
#define APIC_REG_LVT_MACHINE_CHECK      0x02F0ULL
#define APIC_REG_INTERRUPT_COMMAND_LOW  0x0300ULL
#define APIC_REG_INTERRUPT_COMMAND_HIGH 0x0310ULL
#define APIC_REG_LVT_TIMER              0x0320ULL
#define APIC_REG_LVT_THERMAL_SENSOR     0x0330ULL
#define APIC_REG_LVT_PERFORMANCE_MONITOR 0x0340ULL
#define APIC_REG_LVT_LINT0              0x0350ULL
#define APIC_REG_LVT_LINT1              0x0360ULL
#define APIC_REG_LVT_ERROR              0x0370ULL
#define APIC_REG_TIMER_INITIAL_COUNT    0x0380ULL
#define APIC_REG_TIMER_CURRENT_COUNT    0x0390ULL
#define APIC_REG_TIMER_DIVIDE_CONFIG    0x03E0ULL

#define APIC_BASE_MSR                   0x1B

#define APIC_DELIVERY_MODE_FIXED        0x0
#define APIC_DELIVERY_MODE_LOWEST       0x1
#define APIC_DELIVERY_MODE_SMI          0x2
#define APIC_DELIVERY_MODE_NMI          0x4
#define APIC_DELIVERY_MODE_INIT         0x5
#define APIC_DELIVERY_MODE_STARTUP      0x6

#define APIC_DESTINATION_MODE_PHYSICAL  0x0
#define APIC_DESTINATION_MODE_LOGICAL   0x1

#define APIC_DELIVERY_STATUS_IDLE       0x0
#define APIC_DELIVERY_STATUS_PENDING    0x1

#define APIC_LEVEL_DEASSERT             0x0
#define APIC_LEVEL_ASSERT               0x1

#define APIC_TRIGGER_MODE_EDGE          0x0
#define APIC_TRIGGER_MODE_LEVEL         0x1

#define APIC_DESTINATION_SHORTHAND_NONE 0x0
#define APIC_DESTINATION_SHORTHAND_SELF 0x1
#define APIC_DESTINATION_SHORTHAND_ALL  0x2
#define APIC_DESTINATION_SHORTHAND_ALL_EXCLUDING_SELF 0x3

#define apic_read_base_msr(dest)        \
    do {                                \
        asm volatile (                  \
            "rdmsr\n"                   \
            : "=a" (dest.low),          \
              "=d" (dest.high)          \
            : "c" (APIC_BASE_MSR)       \
        );                              \
    } while (0)

#define apic_write_base_msr(src)        \
    do {                                \
        asm volatile (                  \
            "wrmsr\n"                   \
            :                           \
            : "a" (src.low),            \
              "d" (src.high),           \
              "c" (APIC_BASE_MSR)       \
        );                              \
    } while (0)

#define apic_read_reg(base, reg, dest, type) \
    do {                                \
        dest = *((volatile type *)((base) + (reg))); \
    } while (0)

#define apic_write_reg(base, reg, src, type) \
    do {                                \
        *((volatile type *)((base) + (reg))) = (src); \
    } while (0)

union apic_base_msr {
    uint64_t value;

    struct {
        uint32_t low;
        uint32_t high;
    } __attribute__((packed));

    struct {
        uint8_t reserved1 : 8;
        uint8_t bootstrap : 1;
        uint8_t reserved0 : 2;
        uint8_t apic_enable : 1;
        uint64_t apic_base : 52;
    } __attribute__((packed));
};

union apic_id_reg {
    uint32_t value;

    struct {
        uint8_t apic_id;
        uint8_t reserved[3];
    } __attribute__((packed));
};

union apic_version_reg {
    uint32_t value;

    struct {
        uint8_t version;
        uint8_t reserved0;
        uint8_t max_lvt_entries;
        uint8_t eoi_broadcast_suppression;
    } __attribute__((packed));
};

union apic_intr_cmd {
    uint64_t value;

    struct {
        uint32_t low, high;
    } __attribute__((packed));

    struct {
        uint8_t vector;
        uint8_t delivery_mode : 3;
        uint8_t destination_mode : 1;
        uint8_t delivery_status : 1;
        uint8_t reserved0 : 1;
        uint8_t level : 1;
        uint8_t trigger_mode : 1;
        uint8_t reserved1 : 2;
        uint8_t destination_shorthand : 2;
        uint64_t reserved2 : 36;
        uint8_t destination;
    } __attribute__((packed));
};

#endif
