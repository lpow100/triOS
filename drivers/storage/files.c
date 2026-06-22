#include "files.h"

int file_write_text(struct file_descriptor* fd, char* text, uint32_t size)  {
    if (fd -> type != FILE_TEXT && fd -> type != FILE_ISTREAM) return 1;

    uint32_t predicted_cursor = fd -> cursor + size;
    if (predicted_cursor > fd -> size) {
        uint8_t* new_contents = (uint8_t*)malloc(predicted_cursor);
        fd -> size = predicted_cursor;
    }
    
    for (int i = 0; i < size; i ++) {
        fd -> contents[fd -> cursor + i] = text[i];
    }

    return 0;
}
int file_read_text(struct file_descriptor* fd, char* buffer, uint32_t start, uint32_t size) {
    if (fd -> type != FILE_TEXT && fd -> type != FILE_OSTREAM) return 1;
    if (start + size > fd -> size) return 2;

    for (int i = 0; i < size; i ++) {
        buffer[i] = fd -> contents[start + i];
    }

    return 0;
}

int kernel_write_OSTREAM(struct file_descriptor* fd, uint8_t* text, uint32_t size) {
    if (fd -> type != FILE_OSTREAM) return 1;

    uint32_t predicted_cursor = fd -> cursor + size;
    if (predicted_cursor > fd -> size) {
        uint8_t* new_contents = (uint8_t*)malloc(predicted_cursor);
        fd -> size = predicted_cursor;
    }
    
    for (int i = 0; i < size; i ++) {
        fd -> contents[fd -> cursor + i] = text[i];
    }

    return 0;
}

int kernel_read_ISTREAM(struct file_descriptor* fd, uint8_t* buffer, uint32_t start, uint32_t size) {
    if (fd -> type != FILE_ISTREAM) return 1;
    if (start + size > fd -> size) return 2;

    for (int i = 0; i < size; i ++) {
        buffer[i] = fd -> contents[start + i];
    }

    return 0;
}