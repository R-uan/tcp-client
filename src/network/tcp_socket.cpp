#include "network/tcp_socket.h"

#include <arpa/inet.h>
#include <bits/socket.h>
#include <cstring>
#include <netinet/in.h>
#include <sstream>
#include <sys/socket.h>
#include <system_error>
#include <unistd.h>
#include <vector>
#include <poll.h>

#include "network/protocol.h"
#include "utils/logger.hpp"

TcpSocket::TcpSocket(const std::string &addr, const int port) : port(port), address(addr), listening(false) {}

TcpSocket::~TcpSocket() {
    close(this->socket_fd);
    if (this->listening_thread.joinable()) {
        this->listening_thread.join();
    }

    std::stringstream ss;
    ss << "Closing connection with server on port " << this->port << std::endl;
    std::string message = ss.str();
    Logger::info(message);
}

int TcpSocket::connect() {
    std::stringstream ss;
    const int fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd == -1) {
        ss << "Failed to create socket" << std::endl;
        std::string message = ss.str();
        Logger::error(message);
        throw new std::system_error();
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(this->port);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (::connect(fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        ss << "Failed to connect to server" << std::endl;
        std::string message = ss.str();
        Logger::error(message);
        throw new std::system_error();
    }

    this->socket_fd = fd;

    ss << "Connected to server" << std::endl;
    std::string message = ss.str();
    Logger::info(message);

    return 0;
}

int TcpSocket::listen() {
    this->listening = true;
    this->listening_thread = std::thread(&TcpSocket::start_listening, this);

    std::stringstream ss;
    ss << "Listening on port " << this->port << std::endl;
    std::string message = ss.str();
    Logger::info(message);
    return 1;
}

int TcpSocket::send_packet(const Packet &packet) const {
    std::lock_guard lock(this->fd_mutex);
    const ssize_t sent = send(this->socket_fd, &packet.payload(), packet.payload.size(), 0);
    std::stringstream ss;
    ss << "Sending packet of size: " << sent << std::endl;
    std::string message = ss.str();
    Logger::info(message);
    return sent;
}

void TcpSocket::start_listening() {
    char buffer[1024];
    while (this->listening) {
        struct pollfd fds[1];
        fds[0].fd = this->socket_fd;
        fds[0].events = POLLIN;
        fds[0].revents = 0;

        int ret = poll(&fds[0], 1, 1000);
        if (ret == -1) {
            std::stringstream ss;
            ss << "Poll failed" << std::endl;
            std::string message = ss.str();
            Logger::error(message);
            this->listening = false;
            break;
        } else if (ret == 0) {
            continue;
        }

        if (fds[0].revents & POLLIN) {

            std::stringstream ss;
            memset(buffer, 0, sizeof(buffer));
            std::lock_guard lock(this->fd_mutex);
            const int bytes = recv(this->socket_fd, buffer, sizeof(buffer), 0);
            if (bytes == 0) {
                this->listening = false;
                ss << "Connection closed by server " << bytes << std::endl;
                std::string message = ss.str();
                Logger::info(message);
                continue;
            } else if (bytes < 0) {
                this->listening = false;
                ss << "Connection closed due to socket error " << bytes << std::endl;
                std::string message = ss.str();
                Logger::error(message);
                continue;
            }

            const std::vector<uint8_t> packet(buffer, buffer + bytes);
            this->fd_mutex.unlock();
            Protocol::handle_packet(packet);
        }
    }
}
