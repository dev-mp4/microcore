set -e

clang --target=riscv64 -march=rv64i -nostdlib -Wl,-e,_start -o test.elf test.asm
llvm-objcopy -O binary test.elf test.bin