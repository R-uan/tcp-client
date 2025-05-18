#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <cstdint>
#include <optional>
#include <queue>
#include <vector>


struct Packet;
enum class MessageType : uint8_t
{
    DISCONNECT = 0x00,
    CONNECT = 0x01,

    GAMESTATE = 0x10,

    PLAY_CARD = 0x11,
    ATTACK_PLAYER = 0x12,

    ALREADY_CONNECTED = 0xFB,
    INVALID_PLAYER_DATA = 0xFC,
    INVALID_CHECKSUM = 0xFD,
    INVALID_HEADER = 0xFE,
    ERROR = 0xFF,
};

struct Header {
    MessageType message_type;
    uint16_t length;
    uint16_t checksum;

    std::vector<uint8_t> serialize_header() const;
    static std::optional<MessageType> try_from(const uint8_t &value);
    static std::optional<Header> parse_header(const std::vector<uint8_t> &bytes);
    static Header create_header(const MessageType type, const std::vector<uint8_t> &payload);
};

struct Packet {
    Header header;
    std::vector<uint8_t> payload;

    static std::optional<Packet> parse_packet(const std::vector<uint8_t> &bytes);
    static Packet create_packet(const MessageType &type, const std::vector<uint8_t> &payload);
};
#endif //PROTOCOL_H
