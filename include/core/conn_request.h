#pragma once
#include <string>

struct ConnRequest
{
    std::string player_id;
    std::string current_deck_id;
    std::string auth_token;

    ConnRequest(const std::string &player_id, const std::string &current_deck_id, const std::string &auth_token);
};