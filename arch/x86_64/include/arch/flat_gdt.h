#ifndef ARCH_X86_64_FLAT_GDT_H
#define ARCH_X86_64_FLAT_GDT_H

#include <arch/desc.h>

extern union gdt_entry gdt_entries[];
extern struct gdtr gdtr;

#endif // ARCH_X86_64_FLAT_GDT_H