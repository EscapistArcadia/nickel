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

read_root_dentry_prepare:
    // movl .fat32_fsi_next_free_offset(%bx), %eax
    // subl $2, %eax
    movzbl (fat32_sectors_per_cluster), %esi
    mull %esi                                           # %eax = LBA offset of root dentries
    movzwl (fat32_reserved_sector_count), %esi
    // addl %eax, %esi
    movl (fat32_sectors_per_fat_32), %eax
    movzbl (fat32_fat_table_count), %ecx
    mull %ecx
    addl %eax, %esi

read_root_dentry:
    movw $8, %cx
    // movw (root_dentry_segment), %di
    // movw %di, %es
    // xorw %bx, %bx
    // xorw %es, %es
    movw (root_dentry_block), %bx
    call read_disk

each_root_dentry:
    cmpb $0x00, .fat32_dentry_name_offset(%bx)      # checks the end of the list
    je kernel_not_found
    cmpb $0xE5, .fat32_dentry_name_offset(%bx)      # checks the free entry
    je read_root_dentry_next_group

    movw %bx, %si
    movw $kernel_file_name, %di
    movw $.fat32_dentry_attr_offset, %cx
    repe cmpsb                                          # strncmp(%si, %di, %cx)
    jnz next_root_dentry                                # not equal, jump to the next dentry
    jmp kernel_found

next_root_dentry:
    addw .fat32_dentry_size, %bx
    jnc each_root_dentry                                # carry flag is set if overflow

read_root_dentry_next_group:
    addl (root_dentry_block_count), %esi                # read next 8 sectors, unconditionally jump
    jmp read_root_dentry                                # 0xE5: free entry, 0x00: end of the list

kernel_found:                                           # %es:%bx stores the dentry
    

kernel_not_found:
    movw $kernel_not_found_message, %bx
    call print_string
    jmp halt

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
 * @param %esi LBA address of the sector
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

kernel_not_found_message:
    .ascii "Kernel not found.\r\n"
    .byte 0

boot_disk:                                              # the boot drive number
    .byte 0

loader_stack_bottom:                                    # the bottom of the stack
    .word 0x8000

fat32_fs_info_block:                                    # FSINFO structure
    .word 0xA000

root_dentry_block:                                      # %es:%bx = 0xF000 for root dentries
    .word 0xF000

kernel_file_name:                                       # kernel file name in FAT32 format
    .ascii "NICKEL  EXE"

root_dentry_block_count:
    .long 8

fat32_fsi_lead_sig:             .long 0x41615252        # lead signature
fat32_fsi_second_sig:           .long 0x61417272        # second signature
fat32_fsi_end_sig:              .long 0xAA550000        # end signature

.fat32_fsi_lead_sig_offset      = 0x000
.fat32_fsi_second_sig_offset    = 0x1E4
.fat32_fsi_free_count_offset    = 0x1E8
.fat32_fsi_next_free_offset     = 0x1EC
.fat32_fsi_end_sig_offset       = 0x1FC

.fat32_dentry_name_offset       = 0x00
.fat32_dentry_ext_offset        = 0x08
.fat32_dentry_attr_offset       = 0x0B
.fat32_dentry_NTRes_offset      = 0x0C                  # TODO: reserved for Windows NT?
.fat32_dentry_creation_10ms_offset = 0x0D
.fat32_dentry_creation_time_offset = 0x0E
.fat32_dentry_creation_date_offset = 0x10
.fat32_dentry_access_date_offset = 0x12
.fat32_dentry_cluster_high_offset = 0x14
.fat32_dentry_modification_time_offset = 0x16
.fat32_dentry_modification_date_offset = 0x18
.fat32_dentry_cluster_low_offset = 0x1A
.fat32_dentry_file_size_offset  = 0x1C
.fat32_dentry_size              = 32

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
