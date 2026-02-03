#include "hazard.h"

// ... (existing code)

void fetch(HazardControls *ctrl) {
    if (!ctrl->pc_write) {
        printf("[IF] Stalled\n");
        return; // Stall Fetch
    }

    uint32_t instruction = mem_read_word(cpu.pc);
    printf("[IF] PC: 0x%08X | Inst: 0x%08X\n", cpu.pc, instruction);

    if (ctrl->if_id_write) {
        if_id.pc = cpu.pc;
        if_id.inst = instruction;
    }
    
    cpu.next_pc = cpu.pc + 4;
}

void decode(HazardControls *ctrl) {
    uint32_t inst = if_id.inst;
    uint32_t pc = if_id.pc;

    // Flush/Bubble check
    if (ctrl->control_mux_flush) {
        // Insert bubble into ID/EX by zeroing control signals
        memset(&id_ex, 0, sizeof(ID_EX_Latch));
        // Important: preserve PC or set to 0? 0 is fine for bubble.
        printf("[ID] Bubble Inserted\n");
        return;
    }

    // ... (rest of decode logic: same as before but ensure it updates id_ex normally)
    // 1. Decode fields
    uint8_t opcode = get_bits(inst, 6, 0);
    uint8_t rd     = get_bits(inst, 11, 7);
    uint8_t funct3 = get_bits(inst, 14, 12);
    uint8_t rs1    = get_bits(inst, 19, 15);
    uint8_t rs2    = get_bits(inst, 24, 20);
    uint8_t funct7 = get_bits(inst, 31, 25);

    uint32_t rs1_val = (rs1 == 0) ? 0 : cpu.regs[rs1];
    uint32_t rs2_val = (rs2 == 0) ? 0 : cpu.regs[rs2];

    uint32_t imm = 0;
    bool reg_write = false;
    bool mem_read = false;
    bool mem_write = false;
    bool branch = false;
    bool jump = false;

    switch (opcode) {
        case OPCODE_OP_IMM:
            imm = sign_extend(get_bits(inst, 31, 20), 12);
            reg_write = true;
            break;
        case OPCODE_OP:
            reg_write = true;
            break;
        case OPCODE_LUI:
            imm = get_bits(inst, 31, 12) << 12;
            reg_write = true;
            break;
        case OPCODE_STORE:
            imm = sign_extend((get_bits(inst, 31, 25) << 5) | get_bits(inst, 11, 7), 12);
            mem_write = true;
            break;
        case OPCODE_LOAD:
            imm = sign_extend(get_bits(inst, 31, 20), 12);
            reg_write = true;
            mem_read = true;
            break;
        case OPCODE_BRANCH:
            imm = sign_extend(
                (get_bits(inst, 31, 31) << 12) |
                (get_bits(inst, 7, 7) << 11) |
                (get_bits(inst, 30, 25) << 5) |
                (get_bits(inst, 11, 8) << 1),
                13
            );
            branch = true;
            break;
    }

    printf("[ID] Op: %02X | Rd: %d | Imm: %08X\n", opcode, rd, imm);

    id_ex.pc = pc;
    id_ex.rs1_val = rs1_val;
    id_ex.rs2_val = rs2_val;
    id_ex.imm = imm;
    id_ex.rd = rd;
    id_ex.rs1 = rs1;
    id_ex.rs2 = rs2;
    id_ex.opcode = opcode;
    id_ex.func3 = funct3;
    id_ex.func7 = funct7;
    id_ex.reg_write = reg_write;
    id_ex.mem_read = mem_read;
    id_ex.mem_write = mem_write;
    id_ex.branch = branch;
    id_ex.jump = jump;
}

// ALU Helper
static uint32_t alu_op(uint32_t a, uint32_t b, uint8_t funct3, uint8_t funct7, uint8_t opcode) {
    switch (funct3) {
        case FUNC3_ADD: // ADD or SUB
            if (opcode == OPCODE_OP && (funct7 & 0x20)) {
                return a - b;
            }
            return a + b;
        case FUNC3_SLL: return a << (b & 0x1F);
        case FUNC3_SLT: return ((int32_t)a < (int32_t)b) ? 1 : 0;
        case FUNC3_SLTU: return (a < b) ? 1 : 0;
        case FUNC3_XOR: return a ^ b;
        case FUNC3_SRL: // SRL or SRA
             if (funct7 & 0x20) // SRA
                 return (int32_t)a >> (b & 0x1F);
             return a >> (b & 0x1F);
        case FUNC3_OR: return a | b;
        case FUNC3_AND: return a & b;
    }
    return 0;
}

void execute(HazardControls *ctrl) {
    uint32_t rs1 = id_ex.rs1_val;
    uint32_t rs2 = id_ex.rs2_val;
    uint32_t imm = id_ex.imm;
    uint8_t opcode = id_ex.opcode;
    
    // Forwarding Override
    // ctrl->forward_a: 0=REG, 1=WB, 2=EX
    if (ctrl->forward_a == 1)      rs1 = mem_wb.result; 
    else if (ctrl->forward_a == 2) rs1 = ex_mem.alu_result;

    if (ctrl->forward_b == 1)      rs2 = mem_wb.result;
    else if (ctrl->forward_b == 2) rs2 = ex_mem.alu_result;

    // ALU Mux
    uint32_t alu_in1 = rs1;
    uint32_t alu_in2 = rs2;
    
    // ... (rest of execute logic)
    if (opcode == OPCODE_OP_IMM || opcode == OPCODE_LOAD || opcode == OPCODE_STORE || opcode == OPCODE_LUI || opcode == OPCODE_AUIPC) {
        alu_in2 = imm;
    }
    if (opcode == OPCODE_LUI) {
        alu_in1 = 0; 
    }
    if (opcode == OPCODE_AUIPC) {
        alu_in1 = id_ex.pc;
    }

    uint32_t alu_res = 0;
    if (opcode == OPCODE_LUI) {
        alu_res = imm;
    } else {
         alu_res = alu_op(alu_in1, alu_in2, id_ex.func3, id_ex.func7, opcode);
    }
    
    printf("[EX] ALU Res: %08X (FwdA:%d FwdB:%d)\n", alu_res, ctrl->forward_a, ctrl->forward_b);

    ex_mem.alu_result = alu_res;
    ex_mem.rs2_val = rs2; 
    ex_mem.rd = id_ex.rd;
    ex_mem.reg_write = id_ex.reg_write;
    ex_mem.mem_read = id_ex.mem_read;
    ex_mem.mem_write = id_ex.mem_write;
}

void memory_access() {
    uint32_t addr = ex_mem.alu_result;
    uint32_t val = ex_mem.rs2_val;
    uint32_t read_val = 0;

    if (ex_mem.mem_write) {
        printf("[MEM] Write Addr: %08X Val: %08X\n", addr, val);
        mem_write_word(addr, val); 
    }
    
    if (ex_mem.mem_read) {
         read_val = mem_read_word(addr); 
         printf("[MEM] Read Addr: %08X Val: %08X\n", addr, read_val);
    } else {
        read_val = addr; 
    }

    // Update MEM/WB
    mem_wb.result = read_val;
    mem_wb.rd = ex_mem.rd;
    mem_wb.reg_write = ex_mem.reg_write;
}

void write_back() {
    if (mem_wb.reg_write && mem_wb.rd != 0) {
        cpu.regs[mem_wb.rd] = mem_wb.result;
        printf("[WB] Reg x%d <= %08X\n", mem_wb.rd, mem_wb.result);
    }
}

void cpu_step() {
    HazardControls ctrl;
    detect_hazards(&ctrl);

    write_back();
    memory_access();
    execute(&ctrl);
    decode(&ctrl);
    fetch(&ctrl);
    
    if (ctrl.pc_write) {
        cpu.pc = cpu.next_pc;
    }
}
