#ifndef GRUB_H
#define GRUB_H

#include <stdint.h>

struct multiboot_info {
    uint32_t flags;

    // Available if flags[0] set
    uint32_t mem_lower;
    uint32_t mem_upper;

    // Available if flags[1] set
    uint32_t boot_device;

    // Available if flags[2] set
    uint32_t cmdline;

    // Available if flags[3] set
    uint32_t mods_count;
    uint32_t mods_addr;

    // Available if flags[4] or flags[5] set
    uint32_t syms[4];

    // Available if flags[6] set
    uint32_t mmap_length;
    uint32_t mmap_addr;

    // Available if flags[7] set
    uint32_t drives_length;
    uint32_t drives_addr;

    // Available if flags[8] set
    uint32_t config_table;

    // Available if flags[9] set
    uint32_t boot_loader_name;

    // Available if flags[10] set
    uint32_t apm_table;

    // Available if flags[11] set
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;

    // Available if flags[12] set - framebuffer info
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t framebuffer_bpp;
    uint8_t framebuffer_type;
    union {
        struct {
            uint32_t framebuffer_palette_addr;
            uint16_t framebuffer_palette_num_colors;
        };
    };
};


struct multiboot_mmap_entry {
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;
    uint32_t reserved;
} __attribute__((packed));

struct multiboot_mmap {
    uint32_t type;
    uint32_t size;
    uint32_t entry_size;
    uint32_t entry_version;
} __attribute__((packed));

#endif