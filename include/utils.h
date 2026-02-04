#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

int load_binary(const char *filename);
uint32_t sign_extend(uint32_t val, int bits);
uint32_t get_bits(uint32_t val, int high, int low);

#endif
