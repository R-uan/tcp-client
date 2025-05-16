#ifndef CHECKSUM_H
#define CHECKSUM_H
#include <cstdint>
#include <vector>

uint16_t xor_checksum(const std::vector<uint8_t> &data);

#endif //CHECKSUM_H
