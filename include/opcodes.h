#ifndef OPCODES_H
#define OPCODES_H

// RV32I Base Integer Instruction Set Opcodes
#define OPCODE_LUI    0x37
#define OPCODE_AUIPC  0x17
#define OPCODE_JAL    0x6F
#define OPCODE_JALR   0x67
#define OPCODE_BRANCH 0x63
#define OPCODE_LOAD   0x03
#define OPCODE_STORE  0x23
#define OPCODE_OP_IMM 0x13
#define OPCODE_OP     0x33
#define OPCODE_SYSTEM 0x73

// Func3 for Branch
#define FUNC3_BEQ  0x0
#define FUNC3_BNE  0x1
#define FUNC3_BLT  0x4
#define FUNC3_BGE  0x5
#define FUNC3_BLTU 0x6
#define FUNC3_BGEU 0x7

// Func3 for Load
#define FUNC3_LB  0x0
#define FUNC3_LH  0x1
#define FUNC3_LW  0x2
#define FUNC3_LBU 0x4
#define FUNC3_LHU 0x5

// Func3 for Store
#define FUNC3_SB 0x0
#define FUNC3_SH 0x1
#define FUNC3_SW 0x2

// Func3 for OP_IMM and OP
#define FUNC3_ADD  0x0 // ADDI / ADD / SUB
#define FUNC3_SLL  0x1
#define FUNC3_SLT  0x2
#define FUNC3_SLTU 0x3
#define FUNC3_XOR  0x4
#define FUNC3_SRL  0x5 // SRLI / SRAI / SRL / SRA
#define FUNC3_OR   0x6
#define FUNC3_AND  0x7

#endif // OPCODES_H
