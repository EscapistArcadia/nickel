#ifndef __NICKEL_X86_64_APIC_IPI__
#define __NICKEL_X86_64_APIC_IPI__

#include <stdint.h>

extern void apic_send_init_ipi(uint8_t apic_id);
extern void apic_send_startup_ipi(uint8_t apic_id, uint8_t *startup_rip);

#endif
