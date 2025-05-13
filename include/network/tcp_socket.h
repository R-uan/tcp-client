#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H
#include <atomic>
#include <string>
#include <thread>

class TcpSocket {
public:

    int port;
    std::string address;
    std::atomic_bool listening;

    ~TcpSocket();

    TcpSocket(const std::string &addr, int port);
    int connect();
    int listen();

private:
    int socket_fd;
    void start_listening();
    std::thread listening_thread;

};
#endif //TCP_SOCKET_H
