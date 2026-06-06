#include "utils.hpp"

DoubleWord sigext(UDoubleWord value, int from_bits, int to_bits) {
    if (from_bits == 0 || from_bits > 64 || to_bits == 0 || to_bits > 64)
        return 0;

    if (from_bits < 64)
        value &= (1ULL << from_bits) - 1ULL;

    UDoubleWord sign = 1ULL << (from_bits - 1);

    UDoubleWord extended = (value ^ sign) - sign;

    if (to_bits < 64)
        extended &= (1ULL << to_bits) - 1ULL;

    return (UDoubleWord)extended;
}