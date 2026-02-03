#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "memory.h"

int load_binary(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        perror("Failed to open binary file");
        return -1;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (size > MEM_SIZE) {
        fprintf(stderr, "Binary too large for memory\n");
        fclose(f);
        return -1;
    }

    uint8_t *buffer = malloc(size);
    if (!buffer) {
        fprintf(stderr, "Failed to allocate buffer\n");
        fclose(f);
        return -1;
    }

    fread(buffer, 1, size, f);
    fclose(f);

    // Load into memory at 0x0
    for (long i = 0; i < size; i++) {
        mem_write_byte((uint32_t)i, buffer[i]);
    }

    free(buffer);
    printf("Loaded %ld bytes from %s\n", size, filename);
    return 0;
}
