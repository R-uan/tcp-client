#pragma once
#include <string>

#ifndef  PLAYER_CONNECTION_H
#define PLAYER_CONNECTION_H
struct PlayerConnectionRequest
{
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

#endif