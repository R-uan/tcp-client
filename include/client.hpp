#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <cstdint>
#include <sys/types.h>
#include <vector>
struct ClientSocket {
    int socket_fd;

    ClientSocket(const char *ip, int port);
    ~ClientSocket();

    void listen();
    void send_packet(std::vector<uint8_t> packet);
    void handle_incoming_packet(std::vector<uint8_t> &bytes);
};

#endif
