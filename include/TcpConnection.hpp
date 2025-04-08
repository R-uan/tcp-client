#include <atomic>
#include <cstdint>
#include <thread>
#include <vector>
#include <iostream>

#pragma
class TcpConnection
{
public:
    TcpConnection(std::string server_addr, int port);
    ~TcpConnection();

    void connect();
    void start_listening();
    void send_packet(std::vector<uint8_t> packet);
    void handle_incoming_packet(std::vector<uint8_t> &bytes);

private:
    void listen_loop();

    int port;
    int socket_fd;
    std::string server_addr;

    std::thread listen_thread;
    std::atomic_bool listening = false;
};
