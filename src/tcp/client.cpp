#include <cstring>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include "client.hpp"

ClientSocket::ClientSocket(const char *ip, int port) {
        socket_fd = socket(AF_INET, SOCK_STREAM, 0);

        if (socket_fd == -1) {
            std::cerr << "Socket creation failed\n";
            exit(EXIT_FAILURE);
        }

        sockaddr_in serverAddr{};
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

        if (connect(socket_fd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
            std::cerr << "Connection failed\n";
            close(socket_fd);
            exit(EXIT_FAILURE);
        }

        std::cout << "Connected to server!\n";
}

void ClientSocket::send_packet(std::vector<uint8_t> packet) {
    send(socket_fd, packet.data(), packet.size(), 0);
}

ClientSocket::~ClientSocket() {
    close(socket_fd);
    std::cout << "Socket closed." << std::endl;
}

void ClientSocket::listen() {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));

        int bytes_received = recv(socket_fd, buffer, sizeof(buffer), 0);
        if (bytes_received == 0) {
            std::cout << "Connection closed by peer.\n";
            break;
        } else if (bytes_received < 0) {
            std::cerr << "Error receiving data.\n";
            break;
        }

        std::cout << "Received: " << std::string(buffer, bytes_received) << std::endl;
    }
}
