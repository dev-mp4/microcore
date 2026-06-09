#ifndef CORE_HPP
#define CORE_HPP

#include "utils.hpp"
#include <vector>
#include <array>

class Memory {
public:
    Memory(UDoubleWord size);
    ~Memory();

    void reset();

    UByte& operator[](std::size_t idx);

    const UByte& operator[](std::size_t idx) const;

    UDoubleWord size;

    UByte* getRawPtr();

private:
    std::vector<UByte> memory;
};

class CPU {
public:
    UDoubleWord pc = 0;

    std::array<DoubleWord, 32> regs;

    Memory& memory;

    CPU(Memory& memory);
    ~CPU();

    void reset();

    enum InstructionType {
        RV64_ALU_RR_32      =   0b00111011,
        RV64_ALU_RI_32      =   0b00011011,
        RV64_ALU_RR         =   0b00110011,
        RV64_ALU_RI         =   0b00010011,
        RV64_MEM_STORE      =   0b00100011,
        RV64_MEM_LOAD       =   0b00000011,
        RV64_COND_BRANCH    =   0b01100011,
        RV64_JAL            =   0b01101111,
        RV64_JALR           =   0b01100111,
        RV64_LUI            =   0b00110111,
        RV64_AUIPC          =   0b00010111
    };

    struct Instruction {
        UByte opcode;
        UByte rs1;
        UByte rs2;
        UByte rd;
        UByte funct3;
        UByte funct7;
        Word  immediate;
        bool  valid;
    };

    Instruction decode(UWord raw);

    void tick();

    bool checkCondition(DoubleWord a, DoubleWord b, UByte funct3);

    DoubleWord doALU(DoubleWord a, DoubleWord b, UByte funct3, UByte funct7);
};

#endif // CORE_HPP