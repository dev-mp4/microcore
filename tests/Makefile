ARCH=riscv64-unknown-elf
ASM=$(ARCH)-as
LD=$(ARCH)-ld
OBJCOPY=$(ARCH)-objcopy

BUILD_DIR=build

SOURCES_ASM=$(wildcard *.S)
BINARIES_ASM=$(patsubst %.S, $(BUILD_DIR)/%.bin, $(SOURCES_ASM))

.PHONY: all clean

all: $(BINARIES_ASM)
	@echo "--> All tests are built!"

$(BUILD_DIR)/%.bin: %.S
	@echo "--> Building $<"
	@mkdir -p $(BUILD_DIR)
	@$(ASM) -march=rv64gc -o $@.o $<
	@$(LD) -Ttext=0x0 -o $@.elf $@.o
	@$(LD) -Ttext=0x80000000 -N -o $@-spike.elf $@.o
	@$(OBJCOPY) -O binary $@.elf $@
	@rm -rf $@.o

clean:
	@echo "--> Cleaning up"
	@rm -rf $(BUILD_DIR)/*