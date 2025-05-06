#include <memory>
#include <mutex>
#include <nlohmann/json.hpp>

#include "tcp/packet.hpp"
#include "game/player.hpp"
#include "utils/logger.hpp"
#include "game_client_api.h"
#include "tcp/tcp_connection.hpp"

static std::unique_ptr<TcpConnection> g_connection;
static std::mutex g_connection_mutex;

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Player, id, token, current_deck_id, player_color)

extern "C"
{
    API_EXPORT void start_connection(const char *addr, int port, const char *match_id)
    {
        g_connection = std::make_unique<TcpConnection>(std::string(addr), port);
        g_connection->connect();
        g_connection->start_listening();
    }

    API_EXPORT void connect_player(const char *playerId, const char *playerDeckId, const char *token)
    {
        Player player(playerId, token, playerDeckId, "blue");
        auto json_obj = nlohmann::json(player);
        auto cbor_data = nlohmann::json::to_cbor(json_obj);

        Packet connect = Packet::create(MessageType::CONNECT, cbor_data);
        std::vector<uint8_t> packet = connect.wrap_packet();
        std::lock_guard<std::mutex> lock(g_connection_mutex);
        g_connection->send_packet(packet);
    }

    API_EXPORT void free_ptr(uint8_t *ptr)
    {
        delete[] ptr;
    }

    API_EXPORT uint8_t *send_packet(uint8_t messageType, const char *payload, int length)
    {
        std::lock_guard<std::mutex> lock(g_connection_mutex);

        std::vector<uint8_t> payload_vec(payload, payload + length);
        auto type = ProtocolHeader::tryFrom(messageType);
        if (type == std::nullopt)
            return nullptr;

        Packet packet = Packet::create(type.value(), payload_vec);
        g_connection->send_packet(packet.wrap_packet());

        return 0x00;
    }

    API_EXPORT uint8_t *retrieve_gamestate(int *outSize)
    {
        std::stringstream ss;
        ss << "Game State Packets: " << g_connection->gameStateQueue.size();
        std::string message = ss.str();
        Logger::info(message);

        std::lock_guard<std::mutex> lock(g_connection_mutex);

        if (g_connection->gameStateQueue.size() <= 0)
            return nullptr;

        Packet packet = g_connection->gameStateQueue.front();
        std::vector<uint8_t> payload = packet.payload;

        *outSize = payload.size();
        uint8_t *result = new uint8_t[*outSize];
        std::copy(payload.begin(), payload.end(), result);

        g_connection->gameStateQueue.pop();
        return result;
    }

    API_EXPORT uint8_t *retrieve_error(int *outSize)
    {
        std::stringstream ss;
        ss << "Error Packets: " << g_connection->errorQueue.size();
        std::string message = ss.str();
        Logger::info(message);

        if (g_connection->errorQueue.size() <= 0)
            return nullptr;

        Packet packet = g_connection->errorQueue.front();
        std::vector<uint8_t> payload = packet.payload;

        *outSize = payload.size();
        uint8_t *result = new uint8_t[*outSize];
        std::copy(payload.begin(), payload.end(), result);

        g_connection->errorQueue.pop();
        return result;
    }
}
