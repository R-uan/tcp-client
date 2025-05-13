#include "network/protocol.h"

#include <iostream>

#include "synapse_net.h"

#include <bits/ostream.tcc>

void handle_packet(std::vector<uint8_t> bytes) {
    {
        std::shared_lock lock(socket_mutex);
        std::cout << "Pretend I handled the packet." << std::endl;
    }
}
