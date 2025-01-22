/*
 * This snappet of code is to do load the kernel from the file system, and
 * then setup the 32-bit protected mode. We could not setup the 64-bit
 * protected mode here because it requires paging, which is not convenient
 * in assembly.
 *
 * We suppose this code only starts from the bootloader (entry.s).
 */

.code16
.globl _start

_start:                                                 # 0x8000
    popw %ax                                            # saves the boot drive number
    movb %al, (boot_disk)

    movw (loader_stack_bottom), %sp                     # sets the stack pointer
    movw %sp, %bp

read_fs_info:                                           # reads the FSINFO structure
    movw (fat32_fs_info_sector), %si
    movw $1, %cx
    movw (fat32_fs_info_block), %bx
    call read_disk

verify_fs_info:                                         # verifies the FSINFO structure
    movl .fat32_fsi_lead_sig_offset(%bx), %edx
    cmpl (fat32_fsi_lead_sig), %edx
    jne halt
    movl .fat32_fsi_second_sig_offset(%bx), %edx
    cmpl (fat32_fsi_second_sig), %edx
    jne halt
    movl .fat32_fsi_end_sig_offset(%bx), %edx           # only 16 bits for 0xAA55
    cmpl (fat32_fsi_end_sig), %edx
    jne halt



halt:
    hlt
    jmp halt

/* **************************************************
 * *          Helper Functions & Constants          *
 * ************************************************** */

/**
 * @brief prints a null-terminated string to the screen
 *
 * @param %bx the starting address of a null-terminated string
 * @return void
 */
print_string:
    pushw %ax
    movb $0x0E, %ah                                     # %ah = 0x0E: opcode

print_string_iterate:
    movb (%bx), %al                                     # %al = printing character
    cmpb $0, %al                                        # checks termination
    je print_string_end
    
    int $0x10
    incw %bx
    jmp print_string_iterate

print_string_end:
    popw %ax
    ret

/**
 * @brief reads a sector from the disk
 *
 * @param %si LBA address of the sector
 * @param %cx the number of sectors to read
 * @param %es:%bx the memory address to store the sector data
 * @return void
 */
read_disk:
    pushw %ax
    pushw %dx
    pushw %cx

    movw %si, %ax
    movw $0, %dx
    divw (fat32_sectors_per_track)
    incw %dx                                            # sector number
    movw %dx, %cx
    movw $0, %dx
    divw (fat32_number_of_heads)                        # dx = head number
                                                        # ax = cylinder number
    
    movb %dl, %dh
    movb (boot_disk), %dl                               # drive number
    andb $0b00111111, %cl
    movb %al, %ch
    shlb $6, %ah
    orb %ah, %cl
    
    popw %ax
    andw $0b01111111, %ax
    movb $2, %ah
    int $0x13
    jnc read_disk_end
    movw $read_fail_message, %bx
    call print_string
    jmp halt

read_disk_end:
    popw %dx
    popw %ax
    ret

read_fail_message:
    .ascii "Failed to read the disk.\r\n"
    .byte 0

boot_disk:                                              # the boot drive number
    .byte 0

loader_stack_bottom:                                    # the bottom of the stack
    .word 0x8000

fat32_fs_info_block:                                    # FSINFO structure
    .word 0xA000

fat32_fsi_lead_sig:             .long 0x41615252        # lead signature
fat32_fsi_second_sig:           .long 0x61417272        # second signature
fat32_fsi_end_sig:              .word 0xAA550000        # end signature

.fat32_fsi_lead_sig_offset      = 0x000
.fat32_fsi_second_sig_offset    = 0x1E4
.fat32_fsi_free_count_offset    = 0x1E8
.fat32_fsi_next_free_offset     = 0x1EC
.fat32_fsi_end_sig_offset       = 0x1FE

fat32_boot_record:                                      # copy of boot record bootloader
    # fat32_oem_name:             .ascii "NO NAME "       # OEM name identifier
    fat32_bytes_per_sector:     .word 512
    fat32_sectors_per_cluster:  .byte 1
    fat32_reserved_sector_count:.word 32
    fat32_fat_table_count:      .byte 2
    fat32_root_entry_count:     .word 0                 # fixed for FAT32
    fat32_total_sectors_16:     .word 0                 # fixed for FAT32
    # fat32_media_type:           .byte 0xF8              # TODO: what is it?
    # fat32_sectors_per_fat_16:   .word 0                 # fixed for FAT32
    fat32_sectors_per_track:    .word 63                # for int $0x13, TODO: what is it?
    fat32_number_of_heads:      .word 16                # for int $0x13, TODO: what is it?
    # fat32_hidden_sectors:       .long 0                 # for int $0x13, TODO: what is it?
    fat32_total_sectors_32:     .long 262144            # \neq 0 for FAT32
fat32_extended_record:
    fat32_sectors_per_fat_32:   .long 2048
    # fat32_flags:                .word 0
    # fat32_version:              .word 0                 # fixed for FAT32
    fat32_root_cluster:         .long 2
    fat32_fs_info_sector:       .word 1                 # FSINFO structure in reserved area
    fat32_backup_boot_sector:   .word 6                 # backup boot sector in reserved area
    #fat32_reserved:             .long 0
    #                            .long 0
    #                            .long 0
    # fat32_drive_number:         .byte 0x80              # for int $0x13, 0x80 for hard disk
    #fat32_reserved1:            .byte 0
    #fat32_ext_signature:        .byte 0x29              # 0x29 for FAT32
    #fat32_volume_id:            .long 0x67875303        # volume serial number
    #fat32_volume_label:         .ascii "NO NAME    "    # volume label
    #fat32_fs_type:              .ascii "FAT32   "       # file system type
