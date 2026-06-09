#include "core.hpp"
#include "src/args.hpp"
#include "src/utils.hpp"
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <iomanip>
#include <string>

bool loadFirmware(UDoubleWord baseAddress, Memory& memory, std::string filename) {
    std::ifstream file(filename, std::ios::binary);

    if (!file) {
        std::cerr << "Cannot open file: " << filename << std::endl;
        return false;
    }

    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    if (baseAddress > memory.size) {
        std::cerr << "Invalid base address!" << std::endl;
        return false;
    } else if (size + baseAddress > memory.size) {
        std::cerr << "The firmware is too large!" << std::endl;
        return false;
    }

    char byte;
    UDoubleWord offset = baseAddress;
    while (file.get(byte)) {
        memory[offset++] = byte;
    }

    return true;
}

bool dumpMemory(UDoubleWord start, UDoubleWord end, Memory& memory, std::string filename) {
    if (start >= memory.size || end >= memory.size) {
        std::cerr << "Invalid signature start or end!" << std::endl;
        return false;
    }

    std::ofstream file(filename, std::ios::binary);

    if (!file) {
        std::cerr << "Cannot open file: " << filename << std::endl;
        return false;
    }

    UDoubleWord size = end - start + 1;

    file.write((const char*) memory.getRawPtr() + start, size);
    
    return true;
}

int main(int argc, char* argv[]) {
    ArgsParser parser(argc, argv);

    Parameters params;

    try {
        params = parser.parse({1024 * 32, nullptr, 0, false, 0, 0, nullptr});
    } catch (std::runtime_error& e) {
        if (std::strcmp(e.what(), "h") == 0) return 0;

        std::cerr << e.what() << std::endl;
        return 1;
    }

    if (params.firmwareFile == nullptr) {
        std::cerr << "No firmware file specified!" << std::endl;
        return 1;
    }
    
    Memory memory(params.memorySize);
    CPU cpu(memory);

    cpu.reset();

    if (!loadFirmware(0, memory, params.firmwareFile)) {
        return 1;
    }

    if (params.ticksCount == 0) {
        for (;;) {
           cpu.tick();
        }
    } else {
        for (unsigned long long i = 0; i < params.ticksCount; i++) {
            cpu.tick();
        }
    }

    if (params.testMode) {
        // print all 32 registers in hex format
        for (int i = 0; i < 32; i++) {
            std::cout << "x" << std::dec << i << ": 0x" 
                    << std::hex << std::setw(16) << std::setfill('0') 
                    << cpu.regs[i] << std::endl;
        }
        std::cout << "pc: 0x" << std::hex << std::setw(16) << std::setfill('0') 
                << cpu.pc << std::endl;
    }

    if (params.sigFile != nullptr) {
        return dumpMemory(params.sigStart, params.sigEnd, memory, std::string(params.sigFile)) ? 0 : 1;
    }
    return 0;
}