#include "client.hpp"
#include <cstdint>
#include <future>
#include <vector>

uint16_t xor_checksum(const std::vector<uint8_t>& data) {
    uint16_t checksum = 0;
    for (uint8_t byte : data) {
        checksum ^= byte;
    }
    return checksum;
}

enum HeaderType {
    Close = 0x00,
    Connect = 0x01,
    Update = 0x02,
    PlayerConnected = 0x03,
    Err = 0xFF,
};

std::vector<uint8_t> create_header(HeaderType &h_type, std::vector<uint8_t> &body) {
    uint8_t header_type = h_type;
    uint16_t body_len = body.size();
    uint16_t checksum = xor_checksum(body);

    return {
        header_type,
        static_cast<uint8_t>(body_len >> 8),
        static_cast<uint8_t>(body_len & 0xFF),
        static_cast<uint8_t>(checksum >> 8),
        static_cast<uint8_t>(checksum & 0xFF),
        0x0A
    };
}

std::vector<uint8_t> create_packet(HeaderType h_type, std::vector<uint8_t> body) {
    std::vector<uint8_t> header = create_header(h_type, body);

    std::vector<uint8_t> packet(header);
    packet.insert(packet.end(), body.begin(), body.end());

    return packet;
}

int main() {
    ClientSocket client("127.0.0.1", 8000);

    std::future<void> receiver = std::async(std::launch::async, &ClientSocket::listen, &client);

    std::vector<uint8_t> body = {
        0x52, 0x75, 0x61, 0x6E, 0x0A,
        0x52, 0x75, 0x61, 0x6E, 0x0A,
        0x52, 0x75, 0x61, 0x6E, 0x1A,
    };

    std::vector<uint8_t> packet = create_packet(HeaderType::Connect, body);
    client.send_packet(packet);

    receiver.get();

    return 0;
}
