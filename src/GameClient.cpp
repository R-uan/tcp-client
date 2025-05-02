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
}
