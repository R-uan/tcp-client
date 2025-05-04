#include <queue>
#include <thread>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "tcp/packet.hpp"
#include "GameClientAPI.h"
#include "tcp/tcpconnection.hpp"

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
    this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (this->socket_fd == -1)
    {
        std::cerr << "Socket creation failed\n";
        exit(EXIT_FAILURE);
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (::connect(this->socket_fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        std::cerr << "Connection failed\n";
        close(this->socket_fd);
        exit(EXIT_FAILURE);
    }

    std::cout << "Connected to server!\n";
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
            std::cout << "Connection closed by peer.\n";
            break;
        }
        else if (bytes_received < 0)
        {
            std::cerr << "Error receiving data.\n";
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
    send(this->socket_fd, packet.data(), packet.size(), 0);
}

void TcpConnection::handle_incoming_packet(std::vector<uint8_t> &bytes)
{
    std::cout << "Received packet: " << bytes.size() << std::endl;

    Packet packet = Packet::parse(bytes);
    MessageType type = packet.header.header_type;

    switch (type)
    {
    case MessageType::GAMESTATE:
        std::cout << "Game State" << std::endl;
        gameStateQueue.push(packet);
        break;

    case MessageType::ERROR:
        errorQueue.push(packet);
        break;

    default:
        break;
    }
}
