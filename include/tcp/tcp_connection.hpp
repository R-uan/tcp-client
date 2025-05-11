#include <atomic>
#include <cstdint>
#include <thread>
#include <vector>
#include <queue>
#include <iostream>

#pragma


enum NetworkReturnStatus {
    SUCCESS = 0,
    SOCKET_FAILURE = 1,
    CONNECTION_CLOSED = 2,
    INVALID_PACKET = 3,
};

class TcpConnection
{
public:
    std::queue<Packet> errorQueue;
    std::queue<Packet> packetQueue;
    std::queue<Packet> gameStateQueue;

    TcpConnection(std::string server_addr, int port);
    ~TcpConnection();

    int connect();
    int start_listening();
    void queue_packet(const Packet &packet);
    void handle_incoming_packet(std::vector<uint8_t> &bytes);
    [[nodiscard]] ssize_t send_packet(const std::vector<uint8_t>& packet) const;

private:
    void listen_loop();

    int port;
    int socket_fd;
    std::string server_addr;

    std::thread listen_thread;
    std::atomic_bool listening = false;
};
