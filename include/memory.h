#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stddef.h>

#define MEM_SIZE (1024 * 1024) // 1MB simulated RAM

void mem_init();
uint32_t mem_read_word(uint32_t addr);
void mem_write_word(uint32_t addr, uint32_t val);
uint8_t mem_read_byte(uint32_t addr);
void mem_write_byte(uint32_t addr, uint8_t val);

// MMIO mappings
#define MMIO_BASE 0xF0000000
#define MMIO_LED  0xF0000000
#define MMIO_UART 0xF0000004

#endif // MEMORY_H
