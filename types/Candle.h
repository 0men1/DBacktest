#pragma once

#include <iostream>
#include <cstdint>

#pragma pack(push, 1)
struct Candle
{
    uint64_t timestamp;           // 8 bytes
    float open, high, low, close; // 16 bytes
    double volume;                // 8 bytes
    //                             = 32 bytes
};
#pragma pack(pop)

inline std::ostream &operator<<(std::ostream &os, const Candle &c)
{
    os << c.timestamp << ": " << "o: " << c.open << " h: " << c.high << " l: " << c.low << " c: " << c.close << " v: " << c.volume << std::endl;
    return os;
}
