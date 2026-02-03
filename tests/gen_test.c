// This is a C program to generate a binary file for testing because we don't have an assembler.
// Opcode: ADDI (0010011)
// rd=1 (x1), rs1=0 (x0), imm=10
// Inst: 000000001010 00000 000 00001 0010011
// Hex: 00A00093

#include <stdio.h>

int main() {
    FILE *f = fopen("tests/test.bin", "wb");
    if (!f) return 1;
    // ADDI x1, x0, 10 -> 0x00A00093
    // Little Endian: 93 00 A0 00
    unsigned char code[] = {0x93, 0x00, 0xA0, 0x00}; 
    fwrite(code, 1, 4, f);
    
    // ADDI x2, x1, 5 -> x2 = 15
    // imm=5, rs1=1, rd=2
    // 000000000101 00001 000 00010 0010011
    // Hex: 00508113 -> 13 81 50 00
    unsigned char code2[] = {0x13, 0x81, 0x50, 0x00};
    fwrite(code2, 1, 4, f);

    fclose(f);
    return 0;
}
