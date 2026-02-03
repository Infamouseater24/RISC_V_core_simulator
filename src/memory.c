#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint8_t *memory;

void mem_init() {
    memory = (uint8_t *)calloc(MEM_SIZE, 1);
    if (!memory) {
        fprintf(stderr, "Failed to allocate memory\n");
        exit(1);
    }
}

uint32_t mem_read_word(uint32_t addr) {
    if (addr >= MEM_SIZE) {
        // MMIO Read
        if (addr == MMIO_UART) {
            return 0; // Read from UART (simulated empty for now)
        }
        if (addr == MMIO_LED) {
             return 0; // Read LED state (could store it in a static var)
        }
        return 0; 
    }
    // Little endian
    return memory[addr] | (memory[addr+1] << 8) | (memory[addr+2] << 16) | (memory[addr+3] << 24);
}

void mem_write_word(uint32_t addr, uint32_t val) {
    if (addr >= MEM_SIZE) {
        // MMIO Write
        if (addr == MMIO_UART) {
            printf("%c", (char)val); // Simulated UARTTX
        } 
        else if (addr == MMIO_LED) {
            printf("[LED] Output: 0x%08X\n", val);
        }
        return;
    }
    memory[addr] = val & 0xFF;
    memory[addr+1] = (val >> 8) & 0xFF;
    memory[addr+2] = (val >> 16) & 0xFF;
    memory[addr+3] = (val >> 24) & 0xFF;
}

uint8_t mem_read_byte(uint32_t addr) {
     if (addr >= MEM_SIZE) return 0;
     return memory[addr];
}

void mem_write_byte(uint32_t addr, uint8_t val) {
    if (addr >= MEM_SIZE) return;
    memory[addr] = val;
}
