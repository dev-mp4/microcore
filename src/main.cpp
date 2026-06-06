#include "core.hpp"
#include <fstream>
#include <iostream>

bool loadFirmware(UDoubleWord baseAddress, Memory& memory, std::string filename) {
    std::ifstream file(filename, std::ios::binary);

    if (!file) {
        std::cerr << "Cannot open file: " << filename << std::endl;
        return false;
    }

    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    if (baseAddress > Memory::MEM_SIZE) {
        std::cerr << "Invalid base address!" << std::endl;
        return false;
    } else if (size + baseAddress > Memory::MEM_SIZE) {
        std::cerr << "The firmware is too large! Memory size is " << Memory::MEM_SIZE << std::endl;
        return false;
    }

    char byte;
    UDoubleWord offset = baseAddress;
    while (file.get(byte)) {
        memory[offset++] = byte;
    }

    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <firmware binary>" << std::endl;
        return 1;
    }
    
    Memory memory;
    CPU cpu(memory);

    cpu.reset();

    if (!loadFirmware(0, memory, argv[1])) {
        return 1;
    }

    cpu.tick();

    std::cout << cpu.regs[10] << std::endl;
}