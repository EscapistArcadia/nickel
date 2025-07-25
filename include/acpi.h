#ifndef __ARCH_X86_64_INCLUDE_ARCH_ACPI_H__
#define __ARCH_X86_64_INCLUDE_ARCH_ACPI_H__

#include <stdint.h>

#define ACPI_RSDP_SIGNATURE                     "RSD PTR "
#define ACPI_XSDT_SIGNATURE                     "XSDT"
#define ACPI_MADT_SIGNATURE                     "APIC"

#define ACPI_RSDP_REVISION_1                    0
#define ACPI_RSDP_REVISION_2                    2

#define ACPI_XSDT_REVISION                      1
// #define ACPI_MADT_REVISION                      7                                /* TODO: obtained revision (3) does not match the specification */

#define ACPI_MADT_APIC_FLAG_PCAT_COMPATIBLE     0x01

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

#define ACPI_SUCCESS                            0
#define ACPI_FAILURE                            0x80000000
#define ACPI_INVALID_PARAMETER                  (ACPI_FAILURE | 1)
#define ACPI_MISMATCH_CHECKSUM                  (ACPI_FAILURE | 2)
#define ACPI_MISMATCH_SIGNATURE                 (ACPI_FAILURE | 3)
#define ACPI_MISMATCH_REVISION                  (ACPI_FAILURE | 4)
#define ACPI_MISMATCH_CROSSTABLE                (ACPI_FAILURE | 5)
#define ACPI_UNSUPPORTED_VERSION                (ACPI_FAILURE | 6)

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
    // uint32_t length;                                                                /* length of the XSDT in bytes */
    // uint64_t xsdt_address;                                                          /* 64-bit XSDT address (for ACPI 2.0) */
    // uint8_t xdsp_checksum;                                                          /* checksum of the XSDT (including rsdp_checksum) */
    // char reserved[3];
} __attribute__((packed));

struct acpi_xsdp_desc {
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


/**
 * @brief This structure aligns with the MADT (Multiple APIC Description Table) as defined
 *        in the ACPI specification. It contains information about the local APICs and I/O
 *        APICs in the system, which can be used for processor detection and management,
 *        as well as interrupt routing. Note that some fields are reserved.
 */
struct acpi_madt_desc {
    struct acpi_desc_header header;
    uint32_t local_apic_address;                                                   /* Local APIC address */
    uint32_t flags;
    // struct acpi_intr_ctrl_desc entries[0];                                      /* variable length entries, but not pointers */
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
 * @brief Initializes the ACPI subsystem.
 * 
 * @param rsdp Pointer to the RSDP descriptor.
 * @return 0 on success, non-zero on failure.
 */
int32_t acpi_init(struct acpi_xsdp_desc *rsdp);

#endif
