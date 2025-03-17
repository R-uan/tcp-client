#include <future>
#include <vector>
#include <cstdint>
#include "client.hpp"
#include "protocol.hpp"

enum HeaderType {
    Close = 0x00,
    Connect = 0x01,
    Update = 0x02,
    PlayerConnected = 0x03,
    Err = 0xFF,
};

int main() {
    ClientSocket client("127.0.0.1", 8000);

    std::future<void> receiver = std::async(std::launch::async, &ClientSocket::listen, &client);

    std::vector<uint8_t> body = {
        0x52, 0x75, 0x61, 0x6E, 0x0A,
        0x52, 0x75, 0x61, 0x6E, 0x0A,
        0x52, 0x75, 0x61, 0x6E, 0x1A,
    };

    std::vector<uint8_t> packet = Protocol::create_packet(ProtocolOperations::Connect, body);
    client.send_packet(packet);

    receiver.get();

    return 0;
}
