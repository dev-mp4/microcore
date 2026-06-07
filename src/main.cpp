#include "core.hpp"
#include "src/args.hpp"
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>

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

int main(int argc, char* argv[]) {
    ArgsParser parser(argc, argv);

    Parameters params;

    try {
        params = parser.parse({1024 * 32, nullptr, 0, false});
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
        for (int i = 0; i < 31; i++) {
            std::cout << cpu.regs[i] << std::endl;
        }
        std::cout << cpu.pc << std::endl;
    }
}