#include <acpi.h>
#include <cstddef>

// static struct acpi_xsdp_desc *rsdp;
// static struct acpi_rsdt_desc *rsdt;


// static struct acpi_xsdp_desc *xsdp;
// static struct acpi_xsdt_desc *xsdt;

/**
 * @brief Computes the checksum of an ACPI table and verifies it against the provided checksum.
 * 
 * @param table Pointer to the ACPI table.
 * @param size Size of the ACPI table in bytes.
 * @param checksum Expected checksum value.
 * @return 1 if the checksum is valid, 0 otherwise.
 */
static inline int acpi_checksum(const uint8_t *table, uint32_t size, uint8_t checksum) {
    uint8_t sum = 0;
    for (; size > 0; size--, table++) {
        sum += *table;
    }
    return (sum == checksum);
}

static int32_t acpi_parse_madt(const struct acpi_madt_desc *madt) {
    if (madt == NULL) {
        return ACPI_INVALID_PARAMETER;
    } else if (madt->header.revision != ACPI_MADT_REVISION) {
        return ACPI_MISMATCH_REVISION;
    } else if (!acpi_checksum((const uint8_t *)madt, sizeof(struct acpi_madt_desc), madt->header.checksum)) {
        return ACPI_MISMATCH_CHECKSUM;
    } else if (strncmpa(madt->header.signature, ACPI_MADT_SIGNATURE, 4) != 0) {
        return ACPI_MISMATCH_SIGNATURE;
    }

    return ACPI_SUCCESS;
}

static int32_t acpi_parse_rsdt(const struct acpi_rsdt_desc *rsdt) {
    struct acpi_intr_ctrl_desc *entry;

    if (rsdt == NULL) {
        return ACPI_INVALID_PARAMETER;
    } else if (!acpi_checksum((const uint8_t *)rsdt, sizeof(struct acpi_rsdt_desc), rsdt->checksum)) {
        return ACPI_MISMATCH_CHECKSUM;
    }
    
    for (entry = (struct acpi_intr_ctrl_desc *)(rsdt + 1);
         (uintptr_t)entry < (uintptr_t)rsdt + rsdt->length;
         entry = (struct acpi_intr_ctrl_desc *)((uintptr_t)entry + entry->length)) {
        if (entry->type == 0) {

        } else if (entry->type == 1) {

        } else if (entry->type == 2) {

        } else if (entry->type == 3) {

        } else if (entry->type == 4) {

        } else if (entry->type == 5) {

        } else if (entry->type == 6) {
        }
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
    } else if (!acpi_checksum((const uint8_t *)xsdt, sizeof(struct acpi_xsdt_desc), xsdt->checksum)) {
        return ACPI_MISMATCH_CHECKSUM;
    } else if (strncmpa(xsdt->signature, ACPI_XSDT_SIGNATURE, 4) != 0) {
        return ACPI_MISMATCH_SIGNATURE;
    }
    
    entry_count = (xsdt->length - sizeof(struct acpi_xsdt_desc)) / sizeof(uint64_t);
    if (entry_count == 0) {
        return ACPI_MISMATCH_CROSSTABLE;
    }

    for (i = 0; i < entry_count; i++) {
        entry = (struct acpi_desc_header *)(uintptr_t)xsdt->entries[i];
        if (strncmpa(entry->signature, ACPI_MADT_SIGNATURE, 4) == 0
            && (ret = acpi_parse_madt((struct acpi_madt_desc *)entry)) != ACPI_SUCCESS
        ) {
            return ret;
        }
    }

    return ACPI_SUCCESS;
}

/**
 * @brief Initializes the ACPI subsystem.
 * 
 * @param rsdp Pointer to the RSDP descriptor.
 */
int32_t acpi_init(struct acpi_xsdp_desc *rsdp_desc) {
    if (rsdp_desc == NULL) {
        return ACPI_INVALID_PARAMETER;
    } else if (!acpi_checksum((const uint8_t *)rsdp_desc, sizeof(struct acpi_rsdp_desc), rsdp_desc->rsdp_checksum)) {
        return ACPI_MISMATCH_CHECKSUM;
    } else if (strncmpa(rsdp_desc->signature, ACPI_RSDP_SIGNATURE, 8) != 0) {       /* TODO: no longer use provided */
        return ACPI_MISMATCH_SIGNATURE;
    }

    if (rsdp_desc->revision == ACPI_RSDP_REVISION_1) {
        return acpi_parse_rsdt((struct acpi_rsdt_desc *)rsdp_desc->rsdt_address);
    } else if (rsdp_desc->revision == ACPI_RSDP_REVISION_2) {
        return acpi_parse_xsdt((struct acpi_xsdt_desc *)rsdp_desc->xsdt_address);
    } else {
        return ACPI_UNSUPPORTED_VERSION;
    }
}

