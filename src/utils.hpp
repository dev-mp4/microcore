#ifndef UTILS_HPP
#define UTILS_HPP

using Byte = char;
using HalfWord = short;
using Word = int;
using DoubleWord = long long;

using UByte = unsigned char;
using UHalfWord = unsigned short;
using UWord = unsigned int;
using UDoubleWord = unsigned long long;

DoubleWord sigext(UDoubleWord value, int from_bits, int to_bits);

#endif // UTILS_HPP