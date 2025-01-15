/*
 * This file is the entry point for the kernel, the first block (512 bytes) of customizable
 * executable code running by the processor. It is loaded to the memory address 0x7C00.
 * The environment is firstly in 16-bit real mode, and we need to load the next-stage
 * bootloader.
 *
 * The kernel supports FAT32 file system, and we will support more file systems in the future.
 */

.code16
.globl _start

_start:
    jmp init_env
    nop

fat32_boot_record:                                      # required by the file system, PREDEFINED location
    fat32_oem_name:             .ascii "NO NAME "       # OEM name identifier
    fat32_bytes_per_sector:     .word 512
    fat32_sectors_per_cluster:  .byte 1
    fat32_reserved_sector_count:.word 32
    fat32_fat_table_count:      .byte 2
    fat32_root_entry_count:     .word 0                 # fixed for FAT32
    fat32_total_sectors_16:     .word 0                 # fixed for FAT32
    fat32_media_type:           .byte 0xF8              # TODO: what is it?
    fat32_sectors_per_fat_16:   .word 0                 # fixed for FAT32
    fat32_sectors_per_track:    .word 63                # for int $0x13, TODO: what is it?
    fat32_number_of_heads:      .word 16                # for int $0x13, TODO: what is it?
    fat32_hidden_sectors:       .long 0                 # for int $0x13, TODO: what is it?
    fat32_total_sectors_32:     .long 262144            # \neq 0 for FAT32
fat32_extended_record:
    fat32_sectors_per_fat_32:   .long 2048
    fat32_flags:                .word 0
    fat32_version:              .word 0                 # fixed for FAT32
    fat32_root_cluster:         .long 2
    fat32_fs_info_sector:       .word 1                 # FSINFO structure in reserved area
    fat32_backup_boot_sector:   .word 6                 # backup boot sector in reserved area
    fat32_reserved:             .long 0
                                .long 0
                                .long 0
    fat32_drive_number:         .byte 0x80              # for int $0x13, 0x80 for hard disk
    fat32_reserved1:            .byte 0
    fat32_ext_signature:        .byte 0x29              # 0x29 for FAT32
    fat32_volume_id:            .long 0x67875303        # volume serial number
    fat32_volume_label:         .ascii "NO NAME    "    # volume label
    fat32_fs_type:              .ascii "FAT32   "       # file system type

init_env:
    xorw %ax, %ax
    movw %ax, %ds                                       # clears the segment registers
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss

    movb %dl, (boot_disk)                               # save the boot drive number
    
    movw (stack_bottom), %sp                            # set the stack pointer
    movw %sp, %bp

load_stage_2:

boot_disk:                                              # drive number
    .byte 0x00

stack_bottom:                                           # the bottom of the stack in the stage-1.
    .word 0x7C00

.org 0x1FE
    .byte 0x55
    .byte 0xAA
