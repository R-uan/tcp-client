#ifndef CHECKSUM_H
#define CHECKSUM_H
#include <cstdint>
#include <vector>

#include "network/protocol.h"

uint16_t xor_checksum(const std::vector<uint8_t> &data);
bool check_the_sum(const Packet &packet);

#endif //CHECKSUM_H
