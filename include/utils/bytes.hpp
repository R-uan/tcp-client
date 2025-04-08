#ifndef BYTES_HPP
#define BYTES_HPP

#include <cstdint>
#include <vector>

struct u8 {
    static uint16_t to_u16(uint8_t first, uint8_t second);
};

struct u16 {
    static std::vector<uint8_t> to_u8(uint16_t byte);
};

#endif
