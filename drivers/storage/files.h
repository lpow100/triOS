#ifndef FILES_H
#define FILES_H

// TODO: Make stream lose size

#include <stdint.h>
#include <memory/heap.h>

enum file_type {
    FILE_TYPELESS, FILE_TEXT, FILE_ISTREAM, FILE_OSTREAM, FILE_SEF
};

#define MAKE_FILE_MAGIC_NUMBER(type) (((uint32_t)(type) << 20) | 0x3C0DE)
#define GET_FILE_TYPE(magic) ((enum file_type)(magic) >> 20)

#define MAKE_FILE_VERSION(major, minor) (((uint32_t)(major) << 16) | ((uint32_t)(minor) & 0xFFFF))

#define GET_MAJOR_VERSION(version)     ((uint32_t)(version) >> 16)
#define GET_MINOR_VERSION(version)     ((uint32_t)(version) & 0xFFFF)

struct disk_file {
    uint32_t magic;
    uint32_t version;
};

struct file_descriptor {
    uint8_t *contents;
    enum file_type type;
    uint32_t size;
    uint32_t cursor;
};

int file_write_text(struct file_descriptor* fd, char* text, uint32_t size);
int file_read_text(struct file_descriptor* fd, char* buffer, uint32_t start, uint32_t size);

int kernel_write_OSTREAM(struct file_descriptor* fd, uint8_t* text, uint32_t size);
int kernel_read_ISTREAM(struct file_descriptor* fd, uint8_t* buffer, uint32_t start, uint32_t size);

#endif