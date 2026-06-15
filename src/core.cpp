#include <algorithm>
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