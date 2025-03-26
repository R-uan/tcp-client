#include "packet.hpp"
#include <cstdint>
#include <iostream>
#include <optional>
#include "bytes.hpp"
#include <system_error>
#include <vector>

bool ProtocolHeader::check_the_sum(uint16_t &checksum, std::vector<uint8_t> &payload)
{
    uint16_t summy = xor_checksum(payload);
    return summy == checksum;
}

uint16_t ProtocolHeader::xor_checksum(const std::vector<uint8_t> &data)
{
    uint16_t checksum = 0;
    for (uint8_t byte : data)
    {
        checksum ^= byte;
    }

    return checksum;
}

std::optional<MessageType> tryFrom(uint8_t value)
{

    switch (value)
    {
    case 0x00:
        return MessageType::DISCONNECT;
    case 0x01:
        return MessageType::HANDSHAKE;
    case 0xFF:
        return MessageType::ERROR;
    default:
        return std::nullopt;
    }
}

std::vector<uint8_t> ProtocolHeader::wrap_header() const
{
    // Example implementation
    std::vector<uint8_t> header_data;
    header_data.push_back(static_cast<uint8_t>(checksum & 0xFF));
    header_data.push_back(static_cast<uint8_t>((checksum >> 8) & 0xFF));
    header_data.push_back(static_cast<uint8_t>(payload_length & 0xFF));
    header_data.push_back(static_cast<uint8_t>((payload_length >> 8) & 0xFF));
    header_data.push_back(static_cast<uint8_t>(header_type));

    return header_data;
}

std::optional<ProtocolHeader> ProtocolHeader::from_bytes(std::vector<uint8_t> &header_bytes)
{
    if (header_bytes.size() < 5)
        return std::nullopt;
    std::optional<MessageType> header_type = tryFrom(header_bytes[0]);

    std::cout << "Header type: " << &header_bytes[0] << std::endl;
    if (!header_type.has_value())
    {
        std::cout << "Invalid header" << std::endl;
        throw std::errc::protocol_error;
    }

    uint16_t checksum = u8::to_u16(header_bytes[3], header_bytes[4]);
    uint16_t message_length = u8::to_u16(header_bytes[1], header_bytes[2]);

    return ProtocolHeader{
        header_type.value(),
        static_cast<int>(checksum),
        static_cast<int>(message_length)};
}

ProtocolHeader ProtocolHeader::create(MessageType header_type, std::vector<uint8_t> payload)
{
    uint16_t payload_size = payload.size();
    uint16_t checksum = ProtocolHeader::xor_checksum(payload);

    return ProtocolHeader{
        header_type,
        static_cast<int>(checksum),
        static_cast<int>(payload_size)};
}

std::vector<uint8_t> Packet::wrap_packet() const
{
    std::vector<uint8_t> wrapped_header = header.wrap_header();
    std::vector<uint8_t> packet;

    packet.reserve(wrapped_header.size() + payload.size());
    packet.insert(packet.end(), wrapped_header.begin(), wrapped_header.end());
    packet.insert(packet.end(), payload.begin(), payload.end());

    return packet;
}

Packet Packet::parse(std::vector<uint8_t> &protocol)
{
    std::vector<uint8_t> header_bytes(protocol.begin(), protocol.begin() + 5);
    std::optional<ProtocolHeader> header = ProtocolHeader::from_bytes(header_bytes);
    if (header == std::nullopt)
    {
        std::cout << "Invalid Header" << std::endl;
        throw std::errc::protocol_error;
    }

    std::vector<uint8_t> payload_bytes(protocol.begin() + 6, protocol.end());

    return Packet{header.value(), protocol};
}

Packet Packet::create(MessageType header_type, std::vector<uint8_t> &payload)
{
    std::vector<uint8_t> header_bytes(payload.begin(), payload.begin() + 5);
    auto header = ProtocolHeader::create(header_type, payload);
    return Packet{header, payload};
}
