#include "core.hpp"
#include "utils.hpp"

void CPU::tick() {
    UWord raw = memory[pc] | (memory[pc + 1] << 8) | (memory[pc + 2] << 16) | (memory[pc + 3] << 24);

    Instruction instr = decode(raw);
    if (!instr.valid) {
        pc += 4;
        return;
    }

    bool advancePc = true;

    switch (instr.opcode) {
        case RV64_ALU_RR:
            if (instr.rd != 0) regs[instr.rd] = doALU(regs[instr.rs1], regs[instr.rs2], instr.funct3, instr.funct7);
            break;
        case RV64_ALU_RR_32:
            if (instr.rd != 0) regs[instr.rd] = sigext(doALU(regs[instr.rs1] & 0xFFFFFFFF, regs[instr.rs2] & 0xFFFFFFFF, instr.funct3, instr.funct7) & 0xFFFFFFFF, 32, 64);
            break;
        case RV64_ALU_RI:
            if (instr.rd != 0) regs[instr.rd] = doALU(regs[instr.rs1], instr.immediate, instr.funct3, instr.funct7);
            break;
        case RV64_ALU_RI_32:
            if (instr.rd != 0) regs[instr.rd] = sigext(doALU(regs[instr.rs1] & 0xFFFFFFFF, instr.immediate & 0xFFFFFFFF, instr.funct3, instr.funct7) & 0xFFFFFFFF, 32, 64);
            break;
        case RV64_MEM_LOAD:
            if (instr.rd != 0) {
                UDoubleWord address = regs[instr.rs1] + instr.immediate;
                switch (instr.funct3) {
                    case 0b000:
                        regs[instr.rd] = sigext(memory[address], 8, 64);
                        break;
                    case 0b001:
                        regs[instr.rd] = sigext(
                            memory[address] | (memory[address + 1] << 8),
                            16, 64);
                        break;
                    case 0b010:
                        regs[instr.rd] = sigext(
                            memory[address] | (memory[address + 1] << 8) |
                            (memory[address + 2] << 16) | (memory[address + 3] << 24),
                            32, 64);
                        break;
                    case 0b011:
                        regs[instr.rd] = (UDoubleWord)memory[address] | 
                                        ((UDoubleWord)memory[address + 1] << 8)  | 
                                        ((UDoubleWord)memory[address + 2] << 16) | 
                                        ((UDoubleWord)memory[address + 3] << 24) |
                                        ((UDoubleWord)memory[address + 4] << 32) | 
                                        ((UDoubleWord)memory[address + 5] << 40) |
                                        ((UDoubleWord)memory[address + 6] << 48) | 
                                        ((UDoubleWord)memory[address + 7] << 56);
                        break;
                        break;
                    case 0b100:
                        regs[instr.rd] = memory[address];
                        break;
                    case 0b101:
                        regs[instr.rd] = memory[address] | (memory[address + 1] << 8);
                        break;
                    case 0b110:
                        regs[instr.rd] = memory[address] | (memory[address + 1] << 8) |
                            (memory[address + 2] << 16) | (memory[address + 3] << 24);
                        break;
                }
            }
            break;
        case RV64_MEM_STORE: {
            UDoubleWord address = regs[instr.rs1] + instr.immediate;
            switch (instr.funct3) {
                case 0b000:
                    memory[address] = regs[instr.rs2] & 0xFF;
                    break;
                case 0b001:
                    memory[address] = regs[instr.rs2] & 0xFF;
                    memory[address + 1] = (regs[instr.rs2] >> 8) & 0xFF;
                    break;
                case 0b010:
                    memory[address] = regs[instr.rs2] & 0xFF;
                    memory[address + 1] = (regs[instr.rs2] >> 8) & 0xFF;
                    memory[address + 2] = (regs[instr.rs2] >> 16) & 0xFF;
                    memory[address + 3] = (regs[instr.rs2] >> 24) & 0xFF;
                    break;
                case 0b011:
                    memory[address] = regs[instr.rs2] & 0xFF;
                    memory[address + 1] = (regs[instr.rs2] >> 8) & 0xFF;
                    memory[address + 2] = (regs[instr.rs2] >> 16) & 0xFF;
                    memory[address + 3] = (regs[instr.rs2] >> 24) & 0xFF;
                    memory[address + 4] = (regs[instr.rs2] >> 32) & 0xFF;
                    memory[address + 5] = (regs[instr.rs2] >> 40) & 0xFF;
                    memory[address + 6] = (regs[instr.rs2] >> 48) & 0xFF;
                    memory[address + 7] = (regs[instr.rs2] >> 56) & 0xFF;
                    break;
            }
            break;
        }
        case RV64_COND_BRANCH:
            if (checkCondition(regs[instr.rs1], regs[instr.rs2], instr.funct3)) {
                pc += instr.immediate;
                advancePc = false;
            }
            break;
        case RV64_JAL:
            if (instr.rd != 0) regs[instr.rd] = pc + 4;
            pc += instr.immediate;
            advancePc = false;
            break;
        case RV64_JALR: {
            UDoubleWord ret = pc + 4;
            pc = (regs[instr.rs1] + instr.immediate) & ~1;
            if (instr.rd != 0) regs[instr.rd] = ret;
            advancePc = false;
            break;
        }
        case RV64_LUI:
            if (instr.rd != 0) regs[instr.rd] = instr.immediate;
            break;
        case RV64_AUIPC:
            if (instr.rd != 0) regs[instr.rd] = instr.immediate + pc;
            break;
        default:
            break;
    }

    if (advancePc) pc += 4;
    else advancePc = true;
}

bool CPU::checkCondition(DoubleWord a, DoubleWord b, UByte funct3) {
    switch (funct3) {
        case 0b000:
            return a == b;
        case 0b001:
            return a != b;
        case 0b100:
            return a < b;
        case 0b101:
            return a >= b;
        case 0b110:
            return (UDoubleWord) a < (UDoubleWord) b;
        case 0b111:
            return (UDoubleWord) a >= (UDoubleWord) b;
        default:
            return false;
    }
}