# RISC-V Emulator

Another RV64 emulator, written in C++

## Roadmap
- [ ] RV64GC
    - [x] RV64I
    - [x] RV64M
    - [ ] RV64A
    - [ ] RV64F
    - [ ] RV64D
    - [ ] RV64C
- [ ] Privileges
    - [ ] CSRs
    - [ ] Traps and exceptions
    - [ ] Privilege levels
    - [ ] MMU
- [ ] Devices
    - [ ] Framebuffer
    - [ ] CLINT
    - [ ] PLIC
    - [ ] Keyboard
- [ ] Boot
    - [ ] BROM
    - Boot sequence will be planned later, but will probably be similar to MediaTek's one.
- [ ] Running Linux kernel and a shell

## Usage

Compile project with `meson`, executable is located in your build directory, named `mc` (microcore, not Minecraft).

Call `mc --help` to get list of all parameters with their description.

To run programs, you need to compile them to raw RISC-V RV64I binary and run them with  
`mc -fw <path to binary>`

## Technical details

The firmware is loaded and executed from 0x0, the default memory size is 32 KB, but you can modify this by adding `-m` or `--memory` argument with memory size in bytes following.

## Testing

Gemini wrote me a couple assemblies, that test instructions and features, I run them after implementing each feature to know that everything behaves correctly.

You can compile the tests using command `make -C tests all`, but note that you need `riscv64-unknown-elf` binutils and GCC.