#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP
    #include <vector>
    #include <cstdint>
    #include <optional>

    enum class ProtocolType : uint8_t {
        Disconnect = 0x00,
        Connect = 0x01,
        GameState = 0x02,

        Connected = 0x10,
        PlayerMovement = 0x11,

        Err = 0xFF
    };

    std::optional<ProtocolType> tryFrom(uint8_t value);

    struct ProtocolHeader {
        int checksum;
        int message_length;
        ProtocolType operation;

        ProtocolHeader(ProtocolType o, int ml, int cs);
        static ProtocolHeader from_bytes(std::vector<uint8_t> &bytes);
        static uint16_t xor_checksum(const std::vector<uint8_t> &data);
        static bool check_the_sum(uint16_t &checksum, std::vector<uint8_t> &payload);
        static std::vector<uint8_t> create_header(ProtocolType h_type, std::vector<uint8_t> &body);
    };

    struct Protocol {
        ProtocolHeader header;
        std::vector<uint8_t> payload;

        Protocol(std::vector<uint8_t> &protocol);
        static std::vector<uint8_t> create_packet(ProtocolType h_type, std::vector<uint8_t> body);
    };

#endif
