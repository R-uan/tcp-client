#ifndef PACKET_HPP
#define PACKET_HPP

#include <vector>
#include <cstdint>
#include <optional>

enum class MessageType : uint8_t
{
    DISCONNECT = 0x00,
    CONNECT = 0x01,

    GAMESTATE = 0x10,

    PLAYCARD = 0x11,
    ATTACKPLAYER = 0x12,

    ALREADYCONNECTED = 0xFB,
    INVALIDPLAYERDATA = 0xFC,
    INVALIDCHECKSUM = 0xFD,
    INVALIDHEADER = 0xFE,
    ERROR = 0xFF,
};

struct ProtocolHeader
{
    MessageType header_type;
    int checksum;
    int payload_length;

    [[nodiscard]] std::vector<uint8_t> wrap_header() const;
    static std::optional<MessageType> tryFrom(uint8_t value);
    static uint16_t xor_checksum(const std::vector<uint8_t> &data);
    static std::optional<ProtocolHeader> from_bytes(std::vector<uint8_t> &protocol);
    static ProtocolHeader create(MessageType header_type, std::vector<uint8_t> payload);
    static bool check_the_sum(const uint16_t &checksum, const std::vector<uint8_t> &payload);
};

struct Packet
{
    ProtocolHeader header;
    std::vector<uint8_t> payload;

    [[nodiscard]] std::vector<uint8_t> wrap_packet() const;
    static Packet parse(std::vector<uint8_t> &protocol);
    static Packet create(MessageType header_type, std::vector<uint8_t> &payload);
};

#endif
