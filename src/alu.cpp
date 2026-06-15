#include "core.hpp"
#include "utils.hpp"

DoubleWord CPU::doALU(DoubleWord a, DoubleWord b, UByte funct3, UByte funct7) {
    switch (funct3) {
        case 0b000:
            if (funct7 == 0b00000001) {
                return a * b; // pretty enough for MUL
            }
            else return funct7 == 0b0100000 ? a - b : a + b;
        case 0b001:
            if (funct7 == 0b00000001) {
                return ((QuadWord)a * (QuadWord)b) >> 64;
            }
            else return (UDoubleWord) a << (b & 0x3F);
        case 0b010:
            if (funct7 == 0b00000001) {
                return ((QuadWord)a * (QuadWord)(UDoubleWord)b) >> 64;
            }
            else return a < b ? 1 : 0;
        case 0b011:
            if (funct7 == 0b00000001) {
                return ((UQuadWord)(UDoubleWord)a * (UQuadWord)(UDoubleWord)b) >> 64;
            }
            else return (UDoubleWord) a < (UDoubleWord) b ? 1 : 0;
        case 0b100:
            if (funct7 == 0b00000001) {
                if (b == 0) return -1;
                if (a == LLONG_MIN && b == -1) return LLONG_MIN; // prevent host SIGFPE
                return a / b;
            }
            else return a ^ b;
        case 0b101:
            if (funct7 == 0b00000001) {
                if (b == 0) return ULLONG_MAX;
                if (a == LLONG_MIN && b == -1) return LLONG_MIN;
                return (UDoubleWord) a / (UDoubleWord) b;
            }
            else return funct7 == 0b0100000 ? a >> (b & 0x1F) : (UDoubleWord) a >> (b & 0x1F);
        case 0b110:
            if (funct7 == 0b00000001) {
                if (b == 0) return a;
                if (a == LLONG_MIN && b == -1) return 0;
                return a % b;
            }
            else return a | b;
        case 0b111:
            if (funct7 == 0b00000001) {
                if (b == 0) return a;
                if (a == LLONG_MIN && b == -1) return 0;
                return (UDoubleWord) a % (UDoubleWord) b;
            }
            else return a & b;
        default:
            return 0;
    }
}