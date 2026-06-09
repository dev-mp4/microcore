#include <algorithm>
#include <ios>
#include <iostream>
#include <array>
#include "utils.hpp"
#include "core.hpp"

Memory::Memory(UDoubleWord size) : size(size), memory(size, 0) {}
Memory::~Memory() {}

void Memory::reset()  {
    std::fill(memory.begin(), memory.end(), 0);
}

UByte& Memory::operator[](std::size_t idx) {
    return memory[idx];
}

const UByte& Memory::operator[](std::size_t idx) const {
    return memory[idx];
}

UByte* Memory::getRawPtr() {
    return memory.data();
}

CPU::CPU(Memory& memory) : memory(memory) {}
CPU::~CPU() {}

void CPU::reset() {
    pc = 0;
    regs.fill(0);
    memory.reset();
}

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

DoubleWord CPU::doALU(DoubleWord a, DoubleWord b, UByte funct3, UByte funct7) {
    switch (funct3) {
        case 0b000:
            if (funct7 == 0b00000001) {
                return a * b; // pretty enough for MUL
            }
            else return funct7 == 0b0100000 ? a - b : a + b;
        case 0b001:
            if (funct7 == 0b00000001) {
                return ((QuadWord)a * (QuadWord)b) >> 64;
            }
            else return (UDoubleWord) a << (b & 0x3F);
        case 0b010:
            if (funct7 == 0b00000001) {
                return ((QuadWord)a * (QuadWord)(UDoubleWord)b) >> 64;
            }
            else return a < b ? 1 : 0;
        case 0b011:
            if (funct7 == 0b00000001) {
                return ((UQuadWord)(UDoubleWord)a * (UQuadWord)(UDoubleWord)b) >> 64;
            }
            else return (UDoubleWord) a < (UDoubleWord) b ? 1 : 0;
        case 0b100:
            if (funct7 == 0b00000001) {
                if (b == 0) return -1;
                if (a == LLONG_MIN && b == -1) return LLONG_MIN; // prevent host SIGFPE
                return a / b;
            }
            else return a ^ b;
        case 0b101:
            if (funct7 == 0b00000001) {
                if (b == 0) return ULLONG_MAX;
                if (a == LLONG_MIN && b == -1) return LLONG_MIN;
                return (UDoubleWord) a / (UDoubleWord) b;
            }
            else return funct7 == 0b0100000 ? a >> (b & 0x1F) : (UDoubleWord) a >> (b & 0x1F);
        case 0b110:
            if (funct7 == 0b00000001) {
                if (b == 0) return a;
                if (a == LLONG_MIN && b == -1) return 0;
                return a % b;
            }
            else return a | b;
        case 0b111:
            if (funct7 == 0b00000001) {
                if (b == 0) return a;
                if (a == LLONG_MIN && b == -1) return 0;
                return (UDoubleWord) a % (UDoubleWord) b;
            }
            else return a & b;
        default:
            return 0;
    }
}