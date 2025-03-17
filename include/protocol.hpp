#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP
    #include <vector>
    #include <cstdint>
    #include <optional>

    enum class ProtocolOperations : uint8_t {
        Close = 0x00,
        Connect = 0x01,
        Update = 0x02,

        PlayerConnected = 0x10,
        PlayerMovement = 0x11,

        Err = 0xFF
    };

    
    struct Protocol {
        std::optional<ProtocolOperations> tryFrom(uint8_t value);
        
        static uint16_t xor_checksum(const std::vector<uint8_t>& data);
        static std::vector<uint8_t> create_header(ProtocolOperations h_type, std::vector<uint8_t> &body);
        static std::vector<uint8_t> create_packet(ProtocolOperations h_type, std::vector<uint8_t> body);
    };
#endif
