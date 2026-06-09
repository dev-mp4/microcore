#ifndef UTILS_HPP
#define UTILS_HPP

using Byte = char;
using HalfWord = short;
using Word = int;
using DoubleWord = long long;
using QuadWord = __int128;

using UByte = unsigned char;
using UHalfWord = unsigned short;
using UWord = unsigned int;
using UDoubleWord = unsigned long long;
using UQuadWord = unsigned __int128;

constexpr DoubleWord LLONG_MIN   = 0x8000000000000000;
constexpr UDoubleWord ULLONG_MAX = 0xFFFFFFFFFFFFFFFF;

DoubleWord sigext(UDoubleWord value, int from_bits, int to_bits);

#endif // UTILS_HPP