# RISC-V Emulator

Another RV64 emulator, written in C++

## Planned features

- VirtIO GPU(with OpenGL support)
- Framebuffer screen
- Debug UI
- Boot sequence, similar to MediaTek's one

## Usage

Compile project with `meson`, executable is located in your build directory, named `mc` (microcore, not Minecraft).

Call `mc --help` to get list of all parameters with their description.

To run programs, you need to compile them to raw RISC-V RV64I binary and run them with  
`mc -fw <path to binary>`

## Technical details

The firmware is loaded and executed from 0x0, the default memory size is 32 KB, but you can modify this by adding `-m` or `--memory` argument with memory size in bytes following.