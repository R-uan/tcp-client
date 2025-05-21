#include "network/tcp_socket.h"

#include <arpa/inet.h>
#include <bits/socket.h>
#include <cstring>
#include <future>
#include <netinet/in.h>
#include <poll.h>
#include <sstream>
#include <sys/socket.h>
#include <system_error>
#include <unistd.h>
#include <vector>

#include "network/protocol.h"
#include "utils/checksum.h"
#include "utils/logger.hpp"

TcpConnection::TcpConnection(const std::string &addr, const int port) : port(port), address(addr), listening(false) {}

TcpConnection::~TcpConnection() {
    close(this->socket_fd);
    if (this->listening_thread.joinable()) {
        this->listening_thread.join();
    }

    std::stringstream ss;
    ss << "Closing connection with server on port " << this->port << std::endl;
    std::string message = ss.str();
    Logger::info(message);
}

int TcpConnection::connect() {
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

int TcpConnection::listen() {
    this->listening = true;
    this->listening_thread = std::thread(&TcpConnection::start_listening, this);

    std::stringstream ss;
    ss << "Listening on port " << this->port << std::endl;
    std::string message = ss.str();
    Logger::info(message);
    return 1;
}

int TcpConnection::send_packet(const Packet &packet) const {
    std::lock_guard lock(this->fd_mutex);
    const auto packet_bytes = packet.serialize_packet();
    const ssize_t sent = send(this->socket_fd, packet_bytes.data(), packet_bytes.size(), 0);


    std::stringstream ss;
    ss << "Sending packet of size: " << sent << std::endl;
    std::string message = ss.str();
    Logger::info(message);

    return static_cast<int>(sent);
}

void TcpConnection::start_listening() {
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
            auto future = std::async(std::launch::async, [this, packet]() {
                this->handle_packet(packet);
            });
        }
    }
}

void TcpConnection::handle_invalid() const {
    std::string message = "Invalid packet";
    const auto payload = std::vector<uint8_t>(message.begin(), message.end());
    const Packet packet = Packet::create_packet(MessageType::ERROR, payload);
    int _ = this->send_packet(packet);
}

void TcpConnection::handle_game_state(const Packet &packet) {

    std::unique_lock<std::mutex> lock(this->game_state_mutex);
    this->game_state_queue.push(packet);

    std::stringstream ss;
    ss << "Received GAME STATE packet of " << packet.payload.size() << " bytes" << std::endl;
    std::string message = ss.str();
    Logger::info(message);
}

void TcpConnection::handle_packet(const std::vector<uint8_t> &bytes) {
    std::stringstream ss;
    const auto packet = Packet::parse_packet(bytes);
    if (!packet.has_value() || !check_the_sum(packet.value())) {
        handle_invalid();
        return;
    }

    switch (packet.value().header.message_type) {
        case MessageType::GAMESTATE:
            handle_game_state(packet.value());
            break;
        default:
            ss << "Received INVALID packet of " << packet->payload.size() << " bytes" << std::endl;
            std::string message2 = ss.str();
            Logger::info(message2);
            handle_invalid();
            break;
    }
}

TcpConnection* TcpConnection::socket_instance = nullptr;
std::mutex TcpConnection::socket_mutex;

TcpConnection * TcpConnection::GetInstance() {
    std::lock_guard lock(socket_mutex);
    return socket_instance;
}

void TcpConnection::SetInstance(TcpConnection *instance) {
    std::lock_guard lock(socket_mutex);
    if (instance != socket_instance) {
        delete socket_instance;
        socket_instance = instance;

        std::stringstream ss;
        ss << "Set TCP Connection" << std::endl;
        std::string message = ss.str();
        Logger::debug(message);
    }
}
