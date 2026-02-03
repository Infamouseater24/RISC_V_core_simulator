#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"
#include "memory.h"
#include "utils.h"

int main(int argc, char *argv[]) {
    printf("RISC-V Cycle-Accurate Simulator\n");

    if (argc < 2) {
        printf("Usage: %s <binary_file>\n", argv[0]);
        return 1;
    }

    // Initialize components
    mem_init();
    cpu_init();

    if (load_binary(argv[1]) != 0) {
        return 1;
    }

    printf("Simulation starting...\n");
    
    // Main loop
    // TODO: Add exit condition (e.g. magic instruction or max cycles)
    for (int i = 0; i < 100; i++) { // Run 100 cycles for test
        cpu_step();
        // Optional: Dump state
        // printf("Cycle %d complete\n", i);
    }

    printf("Simulation finished.\n");
    return 0;
}
