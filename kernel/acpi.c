#include <acpi.h>
#include <stddef.h>
#include <stdint.h>

static volatile uint32_t cores = 0, enabled_cores = 0;
static volatile struct acpi_processor_local_apic processors[256];

static int strncmp(const char *s1, const char *s2, size_t n) {
    while (n && *s1 && ( *s1 == *s2 )) {
        ++s1;
        ++s2;
        --n;
    }
    return n == 0 ? 0 : ( *(unsigned char *)s1 - *(unsigned char *)s2 );
}

/**
 * @brief Computes the checksum of an ACPI table and verifies it against the provided checksum.
 * 
 * @param table Pointer to the ACPI table.
 * @param size Size of the ACPI table in bytes.
 * @param checksum Expected checksum value.
 * @return 1 if the checksum is valid, 0 otherwise.
 */
static inline int acpi_checksum(const uint8_t *table, uint32_t size) {
    uint8_t sum = 0;
    for (; size > 0; --size, ++table) {
        sum += *table;
    }
    return (sum == 0);
}

static int32_t acpi_parse_madt(const struct acpi_madt_desc *madt) {
    struct acpi_intr_ctrl_desc *entry;

    if (madt == NULL) {
        return ACPI_INVALID_PARAMETER;
    } else if (!acpi_checksum((const uint8_t *)madt, madt->header.length)) {
        return ACPI_MISMATCH_CHECKSUM;
    } else if (strncmp(madt->header.signature, ACPI_MADT_SIGNATURE, 4) != 0) {
        return ACPI_MISMATCH_SIGNATURE;
    }

    for (
        entry = (struct acpi_intr_ctrl_desc *)(madt + 1), cores = 0;
        (size_t)entry - (size_t)madt < madt->header.length;
        entry = (struct acpi_intr_ctrl_desc *)((size_t)entry + entry->length)
    ) {
        if (entry->type == ACPI_MADT_APIC_TYPE_PROCESSOR) {
            processors[cores].uid = entry->processor.uid;
            processors[cores].apic_id = entry->processor.apic_id;
            processors[cores].flags = entry->processor.flags;
            ++cores;
            if (entry->processor.flags & ACPI_PROCESSOR_LOCAL_ENABLED) {
                ++enabled_cores;
            }
        }
    }

    return ACPI_SUCCESS;
}

static int32_t acpi_parse_rsdt(const struct acpi_rsdt_desc *rsdt) {

    if (rsdt == NULL) {
        return ACPI_INVALID_PARAMETER;
    } else if (!acpi_checksum((const uint8_t *)rsdt, rsdt->length)) {
        return ACPI_MISMATCH_CHECKSUM;
    }

    return ACPI_SUCCESS;
}

static int32_t acpi_parse_xsdt(const struct acpi_xsdt_desc *xsdt) {
    int32_t ret;
    uint64_t i, entry_count;
    struct acpi_desc_header *entry;

    if (xsdt == NULL) {
        return ACPI_INVALID_PARAMETER;
    } else if (xsdt->revision != ACPI_XSDT_REVISION) {
        return ACPI_MISMATCH_REVISION;
    } else if (!acpi_checksum((const uint8_t *)xsdt, xsdt->length)) {
        return ACPI_MISMATCH_CHECKSUM;
    } else if (strncmp(xsdt->signature, ACPI_XSDT_SIGNATURE, 4) != 0) {
        return ACPI_MISMATCH_SIGNATURE;
    }
    
    entry_count = (xsdt->length - sizeof(struct acpi_xsdt_desc)) / sizeof(uint64_t);
    if (entry_count == 0) {
        return ACPI_MISMATCH_CROSSTABLE;
    }

    ret = ACPI_SUCCESS;
    for (i = 0; i < entry_count; i++) {
        entry = (struct acpi_desc_header *)(uintptr_t)xsdt->entries[i];
        if (strncmp(entry->signature, ACPI_MADT_SIGNATURE, 4) == 0) {
            if (acpi_parse_madt((struct acpi_madt_desc *)entry) < 0) {
                break;
            }
        }
    }

    return ret;
}

/**
 * @brief Initializes the ACPI subsystem.
 * 
 * @param rsdp Pointer to the RSDP descriptor.
 */
int32_t acpi_init(struct acpi_xsdp_desc *rsdp_desc) {
    if (rsdp_desc == NULL) {
        return ACPI_INVALID_PARAMETER;
    } else if (!acpi_checksum((const uint8_t *)rsdp_desc, sizeof(struct acpi_rsdp_desc))) {
        return ACPI_MISMATCH_CHECKSUM;
    } else if (strncmp(rsdp_desc->signature, ACPI_RSDP_SIGNATURE, 8) != 0) {       /* TODO: no longer use provided */
        return ACPI_MISMATCH_SIGNATURE;
    }

    if (rsdp_desc->revision == ACPI_RSDP_REVISION_1) {
        return acpi_parse_rsdt((struct acpi_rsdt_desc *)(uintptr_t)rsdp_desc->rsdt_address);
    } else if (rsdp_desc->revision == ACPI_RSDP_REVISION_2) {
        if (!acpi_checksum((const uint8_t *)rsdp_desc, sizeof(struct acpi_xsdp_desc))) {
            return ACPI_MISMATCH_CHECKSUM;
        }
        return acpi_parse_xsdt((struct acpi_xsdt_desc *)(uintptr_t)rsdp_desc->xsdt_address);
    } else {
        return ACPI_UNSUPPORTED_VERSION;
    }
}

