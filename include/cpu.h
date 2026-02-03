#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdbool.h>

// Register definitions
#define NUM_REGS 32
#define REG_ZERO 0
#define REG_RA   1
#define REG_SP   2
#define REG_GP   3
#define REG_TP   4

typedef struct {
    uint32_t regs[NUM_REGS];
    uint32_t pc;
    uint32_t next_pc;
} CPU_State;

// Pipeline Latches
typedef struct {
    uint32_t pc;
    uint32_t inst;
} IF_ID_Latch;

typedef struct {
    uint32_t pc;
    uint32_t rs1_val;
    uint32_t rs2_val;
    uint32_t imm;
    uint8_t  rd;
    uint8_t  rs1; // Source Reg 1 Index
    uint8_t  rs2; // Source Reg 2 Index
    uint8_t  opcode;
    uint8_t  func3;
    uint8_t  func7;
    
    // Control Signals
    bool reg_write;
    bool mem_read;
    bool mem_write;
    bool branch;
    bool jump;
} ID_EX_Latch;

typedef struct {
    uint32_t alu_result;
    uint32_t rs2_val; // For store
    uint8_t  rd;
    
    // Control Signals pass-through
    bool reg_write;
    bool mem_read;
    bool mem_write;
} EX_MEM_Latch;

typedef struct {
    uint32_t result; // Read from mem or ALU
    uint8_t  rd;
    
    // Control Signals pass-through
    bool reg_write;
} MEM_WB_Latch;

// Global CPU Instance
extern CPU_State cpu;

// Pipeline Registers
extern IF_ID_Latch if_id;
extern ID_EX_Latch id_ex;
extern EX_MEM_Latch ex_mem;
extern MEM_WB_Latch mem_wb;

// Function Prototypes
void cpu_init();
void cpu_step();

// Forward declaration
struct HazardControls;

// Pipeline Stages
void fetch(struct HazardControls *ctrl);
void decode(struct HazardControls *ctrl);
void execute(struct HazardControls *ctrl);
void memory_access();
void write_back();

#endif // CPU_H
