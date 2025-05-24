#include "synapse_net.h"

#include <iostream>
#include <memory>
#include <mutex>

#include <ostream>
#include <sstream>
#include <nlohmann/json.hpp>
#include "models/player_connection.h"
#include "network/tcp_socket.h"
#include "utils/logger.hpp"

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PlayerConnectionRequest, player_id, auth_token, current_deck_id)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PlayerReconnectionRequest, player_id, auth_token)


API_EXPORT void free_ptr(const uint8_t *ptr) {
    delete[] ptr;
}

API_EXPORT int start_connection(const char *addr, const int port, const char *match_id) {
    try {
        auto* socket = new TcpConnection(std::string(addr), port);
        socket->connect();
        const int listening = socket->listen();
        TcpConnection::SetInstance(socket);
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

API_EXPORT uint8_t *retrieve_game_state(int *outSize) {
    const auto socket = TcpConnection::GetInstance();
    if (socket == nullptr) {
        std::stringstream ss;
        ss << "SOCKET POINTER IS NULL FOR SOME REASON" << std::endl;
        std::string message = ss.str();
        Logger::error(message);
        return nullptr;
    }

    std::stringstream ss;
    ss << "Game State Packets: " << socket->game_state_queue.size() << std::endl;
    std::string message = ss.str();
    Logger::info(message);

    if (socket->game_state_queue.empty()) {
        return nullptr;
    }

    auto [header, payload] = socket->game_state_queue.front();
    *outSize = static_cast<int>(payload.size());
    const auto result = new uint8_t[*outSize];
    std::copy(payload.begin(), payload.end(), result);
    socket->game_state_queue.pop();
    return result;
}

API_EXPORT int play_card(const uint8_t *payload, int length) {
    return 0;
}

API_EXPORT int connect_player(const char *player_id, const char *player_deck_id, const char *auth_token) {
    auto conn = PlayerConnectionRequest(player_id, player_deck_id, auth_token);
    const auto cbor_bytes = nlohmann::json::to_cbor(nlohmann::json(conn));
    const auto packet = Packet::create_packet(MessageType::CONNECT, cbor_bytes);
    const int sent = TcpConnection::GetInstance()->send_packet(packet);
    return sent;
}

API_EXPORT int reconnect_player(const char *player_id, const char *auth_token) {
    auto reconnection = PlayerReconnectionRequest(player_id, auth_token);
    const auto cbor_bytes = nlohmann::json::to_cbor(nlohmann::json(reconnection));
    const auto packet = Packet::create_packet(MessageType::RECONNECT, cbor_bytes);
    const int sent = TcpConnection::GetInstance()->send_packet(packet);
    return sent;
}
