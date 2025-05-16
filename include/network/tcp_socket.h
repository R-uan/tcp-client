#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H
#include <atomic>
#include <mutex>
#include <string>
#include <thread>

#include "protocol.h"

class TcpSocket {
public:

    std::mutex game_state_mutex;
    std::queue<Packet> game_state_queue;

    int port;
    std::string address;
    std::atomic_bool listening;

    ~TcpSocket();

    int listen();
    int connect();
    int send_packet(const Packet &packet) const;
    TcpSocket(const std::string &addr, int port);

private:
    int socket_fd;
    void start_listening();
    mutable std::mutex fd_mutex;
    std::thread listening_thread;
};
#endif //TCP_SOCKET_H
