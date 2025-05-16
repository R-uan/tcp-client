#include "utils/checksum.h"

uint16_t xor_checksum(const std::vector<uint8_t> &data) {
  uint16_t checksum = 0;
    for (const uint8_t byte: data) {
        checksum ^= byte;
    }
    return checksum;
}
