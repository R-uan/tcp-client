#ifndef TCP_SOCKET_H

#define TCP_SOCKET_H

#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <thread>

#include "protocol.h"

class TcpConnection {
public:
    static TcpConnection *GetInstance();
    static void SetInstance(TcpConnection *instance);

    std::mutex game_state_mutex;
    std::queue<Packet> game_state_queue;

    int port;
    std::string address;
    std::atomic_bool listening;

    ~TcpConnection();
    TcpConnection(const std::string &addr, int port);

    int listen();
    int connect();
    int send_packet(const Packet &packet) const;

    void handle_invalid() const;
    void handle_game_state(const Packet &packet);
    void handle_packet(const std::vector<uint8_t> &bytes);

private:
    int socket_fd;
    mutable std::mutex fd_mutex;
    std::thread listening_thread;
    static std::mutex socket_mutex;
    static TcpConnection *socket_instance;

    void start_listening();
};

#endif //TCP_SOCKET_H
