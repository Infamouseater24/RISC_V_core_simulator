#ifndef HAZARD_H
#define HAZARD_H

#include "cpu.h"

typedef struct {
    bool pc_write;
    bool if_id_write;
    bool control_mux_flush; // Insert bubble (stall)
    
    int forward_a; // 0: From Reg, 1: From MEM/WB, 2: From EX/MEM
    int forward_b;
} HazardControls;

void detect_hazards(HazardControls *ctrl);

#endif // HAZARD_H
