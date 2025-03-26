#ifndef PACKET_HPP
#define PACKET_HPP

#include <vector>
#include <cstdint>
#include <optional>

enum class MessageType : uint8_t
{
    DISCONNECT = 0x00,
    HANDSHAKE = 0x01,
    ERROR = 0xFF
};

struct ProtocolHeader
{
    MessageType header_type;
    int checksum;
    int payload_length;

    static uint16_t xor_checksum(const std::vector<uint8_t> &data);
    static bool check_the_sum(uint16_t &checksum, std::vector<uint8_t> &payload);

    std::vector<uint8_t> wrap_header() const;
    static std::optional<ProtocolHeader> from_bytes(std::vector<uint8_t> &protocol);
    static ProtocolHeader create(MessageType header_type, std::vector<uint8_t> payload);
};

struct Packet
{
    ProtocolHeader header;
    std::vector<uint8_t> payload;

    std::vector<uint8_t> wrap_packet() const;
    static Packet parse(std::vector<uint8_t> &protocol);
    static Packet create(MessageType header_type, std::vector<uint8_t> &payload);
};

#endif
