#include <arch/default_idt.h>

static void exp00_divide_error(void) {
    while (1);
}

static void exp01_debug(void) {
    while (1);
}

static void exp02_nmi(void) {
    while (1);
}

static void exp03_breakpoint(void) {
    while (1);
}

static void exp04_overflow(void) {
    while (1);
}

static void exp05_bound_range_exceeded(void) {
    while (1);
}

static void exp06_invalid_opcode(void) {
    asm volatile ("cli");
    asm volatile ("hlt");
    asm volatile ("1: jmp 1b");  // Infinite loop to halt the CPU
    while (1);
}

static void exp07_device_not_available(void) {
    while (1);
}

static void exp08_double_fault(void) {
    while (1);
}

static void exp09_coprocessor_segment_overrun(void) {
    while (1);
}

static void exp0a_invalid_tss(void) {
    while (1);
}

static void exp0b_segment_not_present(void) {
    while (1);
}

static void exp0c_stack_fault(void) {
    while (1);
}

static void exp0d_general_protection(void) {
    while (1);
}

static void exp0e_page_fault(void) {
    while (1);
}

static void exp0f_reserved(void) {
    while (1);
}

static void exp10_x87_floating_point(void) {
    while (1);
}

static void exp11_alignment_check(void) {
    while (1);
}

static void exp12_machine_check(void) {
    while (1);
}

static void exp13_simd_floating_point(void) {
    while (1);
}

static void exp14_virtualization(void) {
    while (1);
}

static void exp15_security(void) {
    while (1);
}

static void exp16_reserved(void) {
    while (1);
}

union idt_entry idt_entries[256];

struct idtr idtr = {
    .limit = sizeof(idt_entries) - 1,
    .base = idt_entries
};

void init_idt() {
    idt_entries[0] = SIMPLE_IDT_ENTRY(0, exp00_divide_error, IVT_INTERRUPT, 0);
    idt_entries[1] = SIMPLE_IDT_ENTRY(1, exp01_debug, IVT_INTERRUPT, 0);
    idt_entries[2] = SIMPLE_IDT_ENTRY(2, exp02_nmi, IVT_INTERRUPT, 0);
    idt_entries[3] = SIMPLE_IDT_ENTRY(3, exp03_breakpoint, IVT_INTERRUPT, 3);
    idt_entries[4] = SIMPLE_IDT_ENTRY(4, exp04_overflow, IVT_INTERRUPT, 3);
    idt_entries[5] = SIMPLE_IDT_ENTRY(5, exp05_bound_range_exceeded, IVT_INTERRUPT, 0);
    idt_entries[6] = SIMPLE_IDT_ENTRY(6, exp06_invalid_opcode, IVT_INTERRUPT, 0);
    idt_entries[7] = SIMPLE_IDT_ENTRY(7, exp07_device_not_available, IVT_INTERRUPT, 0);
    idt_entries[8] = SIMPLE_IDT_ENTRY(8, exp08_double_fault, IVT_INTERRUPT, 0);
    idt_entries[9] = SIMPLE_IDT_ENTRY(9, exp09_coprocessor_segment_overrun, IVT_INTERRUPT, 0);
    idt_entries[10] = SIMPLE_IDT_ENTRY(10, exp0a_invalid_tss, IVT_INTERRUPT, 0);
    idt_entries[11] = SIMPLE_IDT_ENTRY(11, exp0b_segment_not_present, IVT_INTERRUPT, 0);
    idt_entries[12] = SIMPLE_IDT_ENTRY(12, exp0c_stack_fault, IVT_INTERRUPT, 0);
    idt_entries[13] = SIMPLE_IDT_ENTRY(13, exp0d_general_protection, IVT_INTERRUPT, 0);
    idt_entries[14] = SIMPLE_IDT_ENTRY(14, exp0e_page_fault, IVT_INTERRUPT, 0);
    idt_entries[15] = SIMPLE_IDT_ENTRY(15, exp0f_reserved, IVT_INTERRUPT, 0);
    idt_entries[16] = SIMPLE_IDT_ENTRY(16, exp10_x87_floating_point, IVT_INTERRUPT, 0);
    idt_entries[17] = SIMPLE_IDT_ENTRY(17, exp11_alignment_check, IVT_INTERRUPT, 0);
    idt_entries[18] = SIMPLE_IDT_ENTRY(18, exp12_machine_check, IVT_INTERRUPT, 0);
    idt_entries[19] = SIMPLE_IDT_ENTRY(19, exp13_simd_floating_point, IVT_INTERRUPT, 0);
    idt_entries[20] = SIMPLE_IDT_ENTRY(20, exp14_virtualization, IVT_INTERRUPT, 0);
    idt_entries[21] = SIMPLE_IDT_ENTRY(21, exp15_security, IVT_INTERRUPT, 0);
    idt_entries[22] = SIMPLE_IDT_ENTRY(22, exp16_reserved, IVT_INTERRUPT, 0);

    extern void temp_timer_handler(void);
    idt_entries[234] = SIMPLE_IDT_ENTRY(234, temp_timer_handler, IVT_INTERRUPT, 0);
}
