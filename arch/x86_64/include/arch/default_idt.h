#ifndef __NICKEL_X86_64_DEFAULT_IDT_H__
#define __NICKEL_X86_64_DEFAULT_IDT_H__

#include <arch/desc.h>

#define SIMPLE_IDT_ENTRY(id, handler, gate_type, thedpl) \
    (union idt_entry) { \
        offset_15_00: (uint16_t)((uint64_t)(handler) & 0xFFFF), \
        segment: 0x08, /* Kernel Code Segment in GDT */ \
        ist: 0, \
        reserved0: 0, \
        type: gate_type, \
        zero: 0, \
        dpl: thedpl, \
        present: 1, \
        offset_31_16: (uint16_t)(((uint64_t)(handler) >> 16) & 0xFFFF), \
        offset_63_32: (uint32_t)(((uint64_t)(handler) >> 32) & 0xFFFFFFFF), \
        reserved1: 0 \
    }

extern union idt_entry idt_entries[256];
extern struct idtr idtr;

void init_idt(void);

#endif