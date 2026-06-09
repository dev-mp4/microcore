.attribute arch, "rv64im"
.section .text
.global _start

_start:
    # -------------------------------------------------------------------------
    # Setup: Load deliberate test vectors from the data section
    # -------------------------------------------------------------------------
    la    x31, test_data
    ld    x1, 0(x31)         # x1 = -1 (0xFFFFFFFFFFFFFFFF)
    ld    x2, 8(x31)         # x2 = 5
    ld    x3, 16(x31)        # x3 = 0 (Forces division-by-zero validation)
    ld    x4, 24(x31)        # x4 = INT64_MIN (0x8000000000000000)
    ld    x5, 32(x31)        # x5 = 0x1000000020000000 (Checks 32-bit Word instructions)

    # -------------------------------------------------------------------------
    # 1. 64-bit Multiplication (Tests low and high 128-bit splits)
    # -------------------------------------------------------------------------
    mul     x10, x1, x2      # mul:    Low 64-bits of signed -1 * 5
    mulh    x11, x1, x2      # mulh:   High 64-bits of signed -1 * 5
    mulhu   x12, x1, x2      # mulhu:  High 64-bits of unsigned Max * 5
    mulhsu  x13, x1, x2      # mulhsu: High 64-bits of signed -1 * unsigned 5

    # -------------------------------------------------------------------------
    # 2. 64-bit Division & Remainders (Standard paths)
    # -------------------------------------------------------------------------
    div     x14, x1, x2      # div:    Signed -1 / 5
    divu    x15, x1, x2      # divu:   Unsigned Max / 5
    rem     x16, x1, x2      # rem:    Signed -1 % 5
    remu    x17, x1, x2      # remu:   Unsigned Max % 5

    # -------------------------------------------------------------------------
    # 3. Division Edge Cases (Verifies your host crash-protection logic)
    # -------------------------------------------------------------------------
    div     x18, x2, x3      # div by zero:   5 / 0
    divu    x19, x2, x3      # divu by zero:  5 / 0
    div     x20, x4, x1      # div overflow:  INT64_MIN / -1
    rem     x21, x4, x1      # rem overflow:  INT64_MIN % -1

    # -------------------------------------------------------------------------
    # 4. 32-bit Word operations (Ensures sign-extension works correctly)
    # -------------------------------------------------------------------------
    mulw    x22, x5, x2      # mulw:   Multiply lower 32-bits, sign-extend result
    divw    x23, x5, x2      # divw:   Signed 32-bit division, sign-extend result
    divuw   x24, x5, x2      # divuw:  Unsigned 32-bit division, sign-extend result
    remw    x25, x5, x2      # remw:   Signed 32-bit remainder, sign-extend result
    remuw   x26, x5, x2      # remuw:  Unsigned 32-bit remainder, sign-extend result

end_loop:
    beq     x0, x0, end_loop # Trapping loop for your '-tm' step cutoff

# -----------------------------------------------------------------------------
# Test Data Section
# -----------------------------------------------------------------------------
.section .data
.align 3
test_data:
    .quad 0xFFFFFFFFFFFFFFFF   # -1 (Or Unsigned Max)
    .quad 0x0000000000000005   # 5
    .quad 0x0000000000000000   # 0
    .quad 0x8000000000000000   # INT64_MIN
    .quad 0x1000000020000000   # Upper and lower 32-bit target boundaries