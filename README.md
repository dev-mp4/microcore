# RISC-V Emulator

Another RV64 emulator, written in C++

## Planned features

- VirtIO GPU(with OpenGL support)
- Framebuffer screen
- Debug UI
- Boot sequence, similar to MediaTek's one

## Usage

Compile `main.cpp` with any C++ compiler.

Usage syntax: `emu <firmware binary>

Also, you can compile small test program  
Install `clang, lld, LLVM tools` and run `compile_test.sh`

## Technical details

The firmware is loaded and executed from 0x0, the default memory size is 32 KB, but you can modify this in Memory struct by changing value of `MEM_SIZE` constexpr, note that this constant represents size in bytes.