#include <memory>
#include <mutex>
#include <nlohmann/json.hpp>

#include "game/player.hpp"
#include "game_client_api.h"
#include "tcp/packet.hpp"
#include "tcp/tcp_connection.hpp"
#include "utils/logger.hpp"

static std::unique_ptr<TcpConnection> g_connection;
static std::mutex g_connection_mutex;

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Player, id, token, current_deck_id, player_color)

extern "C" {

API_EXPORT void start_connection(const char *addr, int port, const char *match_id) {
    g_connection = std::make_unique<TcpConnection>(std::string(addr), port);
    g_connection->connect();
    g_connection->start_listening();
}

API_EXPORT ssize_t connect_player(const char *playerId, const char *playerDeckId, const char *token) {
    Player player(playerId, token, playerDeckId, "blue");
    const auto json_obj = nlohmann::json(player);
    auto cbor_data = nlohmann::json::to_cbor(json_obj);

    const Packet connect = Packet::create(MessageType::CONNECT, cbor_data);
    const std::vector<uint8_t> packet = connect.wrap_packet();
    std::lock_guard<std::mutex> lock(g_connection_mutex);
    return g_connection->send_packet(packet);
}

API_EXPORT void free_ptr(const uint8_t *ptr) { delete[] ptr; }

API_EXPORT uint8_t *retrieve_gamestate(int *outSize) {
    std::stringstream ss;
    ss << "Game State Packets: " << g_connection->gameStateQueue.size();
    std::string message = ss.str();
    Logger::info(message);

    std::lock_guard<std::mutex> lock(g_connection_mutex);

    if (g_connection->gameStateQueue.empty())
        return nullptr;

    auto [header, payload] = g_connection->gameStateQueue.front();

    *outSize = payload.size();
    auto *result = new uint8_t[*outSize];
    std::copy(payload.begin(), payload.end(), result);

    g_connection->gameStateQueue.pop();
    return result;
}

API_EXPORT uint8_t *retrieve_error(int *outSize) {
    std::stringstream ss;
    ss << "Error Packets: " << g_connection->errorQueue.size();
    std::string message = ss.str();
    Logger::info(message);

    if (g_connection->errorQueue.empty() <= 0)
        return nullptr;

    Packet packet = g_connection->errorQueue.front();
    std::vector<uint8_t> payload = packet.payload;

    *outSize = payload.size();
    auto *result = new uint8_t[*outSize];
    std::copy(payload.begin(), payload.end(), result);

    g_connection->errorQueue.pop();
    return result;
}

API_EXPORT ssize_t play_card(const uint8_t *payload, const int length) {
    std::vector<uint8_t> payload_vector(payload, payload + length);
    const Packet packet = Packet::create(MessageType::PLAYCARD, payload_vector);

    std::lock_guard<std::mutex> lock(g_connection_mutex);
    return g_connection->send_packet(packet.wrap_packet());
}

}
