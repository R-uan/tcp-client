#include "models/requests.h"

#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

PlayerConnectionRequest::PlayerConnectionRequest(const std::string &player_id, const std::string &current_deck_id, const std::string &auth_token)
    : player_id(player_id), current_deck_id(current_deck_id), auth_token(auth_token) {
}

PlayerReconnectionRequest::PlayerReconnectionRequest(const std::string &player_id, const std::string &auth_token)
    : player_id(player_id), auth_token(auth_token) {
}

PlayCardRequest::PlayCardRequest(const std::string &card_id, const std::string &actor_id, const std::optional<std::string> &target_id, const std::optional<std::string> &target_position)
    : card_id(card_id), actor_id(actor_id), target_id(target_id), target_position(target_position) {
}

void assign_optional(nlohmann::json &j, const char *key, const std::optional<std::string> &value) {
    if (value) {
        j[key] = *value;
    } else {
        j[key] = nullptr;
    }
}

nlohmann::json PlayCardRequest::to_json(const PlayCardRequest &request) {
    nlohmann::json j;
    j["card_id"] = request.card_id;
    j["actor_id"] = request.actor_id;

    assign_optional(j, "target_id", request.target_id);
    assign_optional(j, "target_position", request.target_position);

    return j;
}
