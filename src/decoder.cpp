#include "core.hpp"
#include "utils.hpp"
#include <iostream>

CPU::Instruction CPU::decode(UWord raw) {
    Instruction i;
    i.opcode = raw & 0x7F;

    switch (i.opcode) {
        case RV64_ALU_RR_32:
        case RV64_ALU_RR:
            i.rd = (raw >> 7) & 0x1F;
            i.funct3 = (raw >> 12) & 0x07;
            i.rs1 = (raw >> 15) & 0x1F;
            i.rs2 = (raw >> 20) & 0x1F;
            i.funct7 = (raw >> 25) & 0x7F;
            break;
        case RV64_ALU_RI_32:
            i.rd = (raw >> 7) & 0x1F;
            i.funct3 = (raw >> 12) & 0x07;
            i.rs1 = (raw >> 15) & 0x1F;
            if (i.funct3 == 0b001 || i.funct3 == 0b101) {
                i.immediate = (raw >> 20) & 0x1F;
                i.funct7 = (raw >> 25) & 0x7F;
            } else {
                i.immediate = sigext((raw >> 20) & 0xFFF, 12, 64);
            }
            break;
        case RV64_ALU_RI:
            i.rd = (raw >> 7) & 0x1F;
            i.funct3 = (raw >> 12) & 0x07;
            i.rs1 = (raw >> 15) & 0x1F;
            if (i.funct3 == 0b001 || i.funct3 == 0b101) {
                i.immediate = (raw >> 20) & 0x3F;
                i.funct7 = ((raw >> 26) & 0x3F) << 1;
            } else {
                i.immediate = sigext((raw >> 20) & 0xFFF, 12, 64);
            }
            break;
        case RV64_JALR:
        case RV64_MEM_LOAD:
            i.rd = (raw >> 7) & 0x1F;
            i.funct3 = (raw >> 12) & 0x07;
            i.rs1 = (raw >> 15) & 0x1F;
            i.immediate = sigext((raw >> 20) & 0xFFF, 12, 64);
            break;
        case RV64_MEM_STORE:
            i.funct3 = (raw >> 12) & 0x07;
            i.rs1 = (raw >> 15) & 0x1F;
            i.rs2 = (raw >> 20) & 0x1F;
            i.immediate = sigext(
                ((((raw >> 25) & 0x7F) << 5) | ((raw >> 7) & 0x1F)),
                    12, 64);
            break;
        case RV64_COND_BRANCH:
            i.funct3 = (raw >> 12) & 0x07;
            i.rs1 = (raw >> 15) & 0x1F;
            i.rs2 = (raw >> 20) & 0x1F;
            i.immediate = sigext(
                (((raw >> 8) & 0xF) << 1) | (((raw >> 25) & 0x3F) << 5) | (((raw >> 7) & 0x1) << 11) | (((raw >> 31) & 0x1) << 12),
            13, 64);
            break;
        case RV64_JAL:
            i.rd = (raw >> 7) & 0x1F;
            i.immediate = sigext(
                (((raw >> 31) & 0x1) << 20)
                | (((raw >> 21) & 0x3FF) << 1)
                | (((raw >> 20) & 0x1) << 11)
                | (((raw >> 12) & 0xFF) << 12),
                21, 64
            );
            break;
        case RV64_AUIPC:
        case RV64_LUI:
            i.rd = (raw >> 7) & 0x1F;
            i.immediate = ((raw >> 12) & 0xFFFFF) << 12;
            break;
        default:
            std::cerr << "Invalid opcode: 0x" << std::hex << (UWord) i.opcode << std::dec << std::endl;
            i.valid = false;
            return i;
    }
    
    i.valid = true;
    return i;
}