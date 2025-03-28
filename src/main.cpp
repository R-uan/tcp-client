#include <future>
#include <vector>
#include <cstdint>
#include <iostream>
#include "client.hpp"
#include "packet.hpp"

int main()
{
    ClientSocket client("127.0.0.1", 8000);

    std::future<void> receiver = std::async(std::launch::async, &ClientSocket::listen, &client);

    std::string omegalul("e2372808-8c1f-4e1b-94b2-6a70ebaf5029\nyabadabadoo");
    std::vector<uint8_t> body(omegalul.begin(), omegalul.end());

    Packet packet = Packet::create(MessageType::CONNECT, body);
    client.send_packet(packet.wrap_packet());

    receiver.get();

    return 0;
}
