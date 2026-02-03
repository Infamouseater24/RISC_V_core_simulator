#include "hazard.h"
#include <stdio.h>

// Forwarding Mux Codes
#define FWD_REG 0
#define FWD_WB  1 // Forward from MEM/WB stage (Distance 2)
#define FWD_EX  2 // Forward from EX/MEM stage (Distance 1)

void detect_hazards(HazardControls *ctrl) {
    // Defaults
    ctrl->pc_write = true;
    ctrl->if_id_write = true;
    ctrl->control_mux_flush = false;
    
    // Forwarding Defaults: No forwarding
    ctrl->forward_a = FWD_REG;
    ctrl->forward_b = FWD_REG;

    // --- Forwarding Unit ---
    
    // Forward A (rs1)
    if (ex_mem.reg_write && (ex_mem.rd != 0) && (ex_mem.rd == id_ex.rs1)) {
        ctrl->forward_a = FWD_EX; // Forward from EX/MEM
    } else if (mem_wb.reg_write && (mem_wb.rd != 0) && (mem_wb.rd == id_ex.rs1)) {
        ctrl->forward_a = FWD_WB; // Forward from MEM/WB
    }

    // Forward B (rs2)
    if (ex_mem.reg_write && (ex_mem.rd != 0) && (ex_mem.rd == id_ex.rs2)) {
        ctrl->forward_b = FWD_EX;
    } else if (mem_wb.reg_write && (mem_wb.rd != 0) && (mem_wb.rd == id_ex.rs2)) {
        ctrl->forward_b = FWD_WB;
    }

    // --- Load-Use Hazard Detection ---
    // If ID/EX is a Load and ID/EX.rd matches IF/ID.rs1 or IF/ID.rs2
    // We need IF/ID latch to check next instruction dependency *before* it gets to EX.
    // However, our `id_ex` struct represents the instruction currently in EX stage (after decode),
    // and `if_id` represents the instruction in ID stage.
    //
    // WAIT: variable naming convention check.
    // In `cpu.c`, `cpu_step` calls `write_back`, `memory`, `execute`, `decode`, `fetch`.
    // `decode` reads `if_id` and writes `id_ex`.
    // So right before `decode` runs, `id_ex` holds the *previous* instruction (now in EX), 
    // and `if_id` holds the *current* instruction (now in ID).
    //
    // Detection Trigger:
    // If (ID/EX.MemRead) and ((ID/EX.rd == IF/ID.rs1) or (ID/EX.rd == IF/ID.rs2))
    
    // Decoding fields from IF/ID raw instruction for hazard check
    uint32_t inst = if_id.inst;
    uint8_t id_rs1 = (inst >> 15) & 0x1F;
    uint8_t id_rs2 = (inst >> 20) & 0x1F;

    if (id_ex.mem_read && ((id_ex.rd == id_rs1) || (id_ex.rd == id_rs2))) {
        // Stall
        ctrl->pc_write = false;
        ctrl->if_id_write = false;
        ctrl->control_mux_flush = true; // Flush ID_EX (insert bubble) logic needs to be handled in Decode
        // Actually, flushing usually means setting Control Signals to 0 in ID/EX latch.
    }
}
