#include <queue>
#include <thread>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "tcp/packet.hpp"
#include "utils/logger.hpp"
#include "game_client_api.h"
#include "tcp/tcp_connection.hpp"

TcpConnection::TcpConnection(std::string server_addr, int port)
    : server_addr(server_addr), port(port) {}

TcpConnection::~TcpConnection()
{
    std::cout << "Closing connection by destructor." << std::endl;
    close(socket_fd);
    if (this->listen_thread.joinable())
    {
        this->listen_thread.join();
    }
}

void TcpConnection::connect()
{
    std::stringstream ss;
    this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (this->socket_fd == -1)
    {
        ss << "Socket creation failed" << std::endl;
        std::string message = ss.str();
        Logger::error(message);
        exit(EXIT_FAILURE);
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (::connect(this->socket_fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        ss << "Connection failed" << std::endl;
        std::string message = ss.str();
        Logger::error(message);

        close(this->socket_fd);
        exit(EXIT_FAILURE);
    }

    ss << "Connected to server: " << this->server_addr << ":" << this->port << std::endl;
    std::string message = ss.str();
    Logger::info(message);
}

void TcpConnection::listen_loop()
{
    char buffer[1024];
    while (true)
    {
        memset(buffer, 0, sizeof(buffer));

        int bytes_received = recv(this->socket_fd, buffer, sizeof(buffer), 0);
        if (bytes_received == 0)
        {
            std::stringstream ss;
            ss << "Connection closed by the server." << std::endl;
            std::string message = ss.str();
            Logger::info(message);
            break;
        }
        else if (bytes_received < 0)
        {
            std::stringstream ss;
            ss << "Error receiving data from the server." << std::endl;
            std::string message = ss.str();
            Logger::info(message);
            break;
        }

        std::vector<uint8_t> bytes(buffer, buffer + bytes_received);
        this->handle_incoming_packet(bytes);
    }
}

void TcpConnection::start_listening()
{
    this->listening = true;
    std::cout << "Listening to the socket" << std::endl;
    this->listen_thread = std::thread(&TcpConnection::listen_loop, this);
}

void TcpConnection::queue_packet(Packet packet)
{
    packetQueue.push(packet);
}

void TcpConnection::send_packet(std::vector<uint8_t> packet)
{
    ssize_t sent = send(this->socket_fd, packet.data(), packet.size(), 0);
    std::stringstream ss;
    ss << "Sent " << sent << " bytes." << std::endl;
    std::string message = ss.str();
    Logger::info(message);
}

void TcpConnection::handle_incoming_packet(std::vector<uint8_t> &bytes)
{
    std::stringstream ss;

    Packet packet = Packet::parse(bytes);
    MessageType type = packet.header.header_type;

    switch (type)
    {
    case MessageType::GAMESTATE:
        ss << "Received game state of " << bytes.size() << " bytes." << std::endl;
        gameStateQueue.push(packet);
        break;

    case MessageType::ERROR:
        ss << "Received error of " << bytes.size() << " bytes." << std::endl;
        errorQueue.push(packet);
        break;

    default:
        ss << "Received unkwown of " << bytes.size() << " bytes." << std::endl;
        break;
    }

    std::string message = ss.str();
    Logger::info(message);
}
