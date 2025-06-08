#pragma once
#include <optional>
#include <string>
#include <nlohmann/json_fwd.hpp>

#ifndef REQUESTS_H
#define REQUESTS_H

struct PlayerConnectionRequest {
    std::string player_id;
    std::string current_deck_id;
    std::string auth_token;

    PlayerConnectionRequest(const std::string &player_id, const std::string &current_deck_id, const std::string &auth_token);
};

struct PlayerReconnectionRequest {
    std::string player_id;
    std::string auth_token;

    PlayerReconnectionRequest(const std::string &player_id, const std::string &auth_token);
};

struct PlayCardRequest {
    std::string card_id;
    std::string actor_id;
    std::optional<std::string> target_id;
    std::optional<std::string> target_position;;

    PlayCardRequest(const std::string &card_id,
                    const std::string &actor_id,
                    const std::optional<std::string> &target_id = std::nullopt,
                    const std::optional<std::string> &target_position = std::nullopt);

    static nlohmann::json to_json(const PlayCardRequest &request);
};


#endif
