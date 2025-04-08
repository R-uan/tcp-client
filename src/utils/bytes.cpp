#include <cstdint>
#include <vector>

#include "utils/bytes.hpp"

uint16_t u8::to_u16(uint8_t first, uint8_t second)
{
    return (static_cast<uint16_t>(first) << 8) | second;
};

std::vector<uint8_t> u16::to_u8(uint16_t byte)
{
    return {
        static_cast<uint8_t>(byte & 0xFF),
        static_cast<uint8_t>((byte << 8) & 0xFF),
    };
}
