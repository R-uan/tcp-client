#include <memory>
#include <mutex>
#include <nlohmann/json.hpp>

#include "tcp/packet.hpp"
#include "game/player.hpp"
#include "GameClientAPI.h"
#include "tcp/tcpconnection.hpp"

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

    API_EXPORT uint8_t *get_next_gamestate(int *outSize)
    {
        std::cout << "GameState packets: " << g_connection->gameStateQueue.size() << std::endl;
        std::lock_guard<std::mutex> lock(g_connection_mutex);
        if (g_connection->gameStateQueue.size() <= 0)
            return nullptr;
        Packet packet = g_connection->gameStateQueue.front();
        std::vector<uint8_t> payload = packet.payload;
        std::cout << "1" << std::endl;
        *outSize = payload.size();
        std::cout << "2" << std::endl;
        uint8_t *result = new uint8_t[*outSize];
        std::cout << "3" << std::endl;
        std::copy(payload.begin(), payload.end(), result);
        std::cout << "4" << std::endl;
        g_connection->gameStateQueue.pop();
        std::cout << "5" << std::endl;
        return result;
    }
}
