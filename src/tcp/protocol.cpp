#include <cmath>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <iterator>
#include <optional>
#include <ostream>
#include <system_error>
#include <vector>
#include "bytes.hpp"
#include "protocol.hpp"

ProtocolHeader::ProtocolHeader(ProtocolType o, int ml, int cs) : operation(o), checksum(cs), message_length(ml) {}

std::optional<ProtocolType> tryFrom(uint8_t value) {

    switch (value) {
        case 0x00: return ProtocolType::Disconnect;
        case 0x01: return ProtocolType::Connect;
        case 0x02: return ProtocolType::GameState;
        case 0x10: return ProtocolType::Connected;
        case 0x11: return ProtocolType::PlayerMovement;
        default: return std::nullopt;
    }
}

std::vector<uint8_t> ProtocolHeader::create_header(ProtocolType h_type, std::vector<uint8_t> &payload) {
    uint16_t payload_size = payload.size();
    uint16_t checksum = xor_checksum(payload);
    uint8_t header_type = static_cast<uint8_t>(h_type);

    return {
        header_type,
        static_cast<uint8_t>(payload_size >> 8),
        static_cast<uint8_t>(payload_size & 0xFF),
        static_cast<uint8_t>(checksum >> 8),
        static_cast<uint8_t>(checksum & 0xFF),
        0x0A
    };
}

ProtocolHeader ProtocolHeader::from_bytes(std::vector<uint8_t> &bytes) {
    std::vector<uint8_t> payload_bytes(bytes.begin() + 6, bytes.end());
    std::vector<uint8_t> header_bytes(bytes.begin(), bytes.begin() + 5);

    std::optional<ProtocolType> operation = tryFrom(header_bytes[0]);
    if (!operation.has_value()) {
        std::cout << "Invalid operation" << std::endl;
        throw std::errc::protocol_error;
    }

    uint16_t checksum = u8::to_u16(header_bytes[3], header_bytes[4]);
    uint16_t message_length = u8::to_u16(header_bytes[1], header_bytes[2]);

    if(!check_the_sum(checksum, payload_bytes)) {
        std::cout << "Checksum failed" << std::endl;
        throw std::errc::protocol_error;
    }

    ProtocolHeader header(operation.value(), static_cast<int>(message_length), static_cast<int>(checksum));
    return header;
}

bool ProtocolHeader::check_the_sum(uint16_t &checksum, std::vector<uint8_t> &payload) {
    uint16_t summy = xor_checksum(payload);
    return summy == checksum;
}

uint16_t ProtocolHeader::xor_checksum(const std::vector<uint8_t> &data) {
    uint16_t checksum = 0;
    for (uint8_t byte : data) {
        checksum ^= byte;
    }
    return checksum;
}

std::vector<uint8_t> Protocol::create_packet(ProtocolType h_type, std::vector<uint8_t> body) {
    std::vector<uint8_t> header = ProtocolHeader::create_header(h_type, body);

    std::vector<uint8_t> packet(header);
    packet.insert(packet.end(), body.begin(), body.end());

    return packet;
}

Protocol::Protocol(std::vector<uint8_t> &protocol) :
    header(ProtocolHeader::from_bytes(protocol)),
    payload(protocol.begin() + 6, protocol.end()) {}
