#include <cstdint>
#include <optional>
#include <vector>
#include "protocol.hpp"

std::optional<ProtocolOperations> tryFrom(uint8_t value) {
    switch (value) {
        case 0x00: return ProtocolOperations::Close;
        case 0x01: return ProtocolOperations::Connect;
        case 0x02: return ProtocolOperations::Update;
        case 0x10: return ProtocolOperations::PlayerConnected;
        case 0x11: return ProtocolOperations::PlayerMovement;
        default: return std::nullopt;
    }
}

std::vector<uint8_t> Protocol::create_header(ProtocolOperations h_type, std::vector<uint8_t> &body) {
    uint16_t body_len = body.size();
    uint16_t checksum = xor_checksum(body);
    uint8_t header_type = static_cast<uint8_t>(h_type);
    
    return {
        header_type,
        static_cast<uint8_t>(body_len >> 8),
        static_cast<uint8_t>(body_len & 0xFF),
        static_cast<uint8_t>(checksum >> 8),
        static_cast<uint8_t>(checksum & 0xFF),
        0x0A
    };
}

uint16_t Protocol::xor_checksum(const std::vector<uint8_t>& data) {
    uint16_t checksum = 0;
    for (uint8_t byte : data) {
        checksum ^= byte;
    }
    return checksum;
}

std::vector<uint8_t> Protocol::create_packet(ProtocolOperations h_type, std::vector<uint8_t> body) {
    std::vector<uint8_t> header = create_header(h_type, body);
    
    std::vector<uint8_t> packet(header);
    packet.insert(packet.end(), body.begin(), body.end());
    
    return packet;
}
    