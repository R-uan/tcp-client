#include "synapse_net.h"
#include <memory>
#include <mutex>

#include <ostream>
#include <sstream>

#include "network/tcp_socket.h"
#include "utils/logger.hpp"


API_EXPORT void free_ptr(const uint8_t *ptr) {}

API_EXPORT int start_connection(const char *addr, const int port, const char *match_id) {
    try {
        socket_ptr = std::make_unique<TcpSocket>(std::string(addr), port);
        socket_ptr->connect();
        const int listening = socket_ptr->listen();
        return listening;
    } catch (...) {
        std::stringstream ss;
        ss << "Failed to create socket" << std::endl;
        std::string message = ss.str();
        Logger::error(message);
        return -1;
    }
}


API_EXPORT uint8_t *retrieve_error(int *outSize) {
    return nullptr;
}


API_EXPORT uint8_t *retrieve_gamestate(int *outSize) {
    return nullptr;
}


API_EXPORT int play_card(const uint8_t *payload, int length) {
    return 0;
}


API_EXPORT int connect_player(const char *playerId, const char *playerDeckId, const char *token) {
    return 0;
}
