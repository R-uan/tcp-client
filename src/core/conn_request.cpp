#include "core/conn_request.h"

ConnRequest::ConnRequest(const std::string &player_id, const std::string &current_deck_id, const std::string &auth_token)
    : player_id(player_id), current_deck_id(current_deck_id), auth_token(auth_token) {}
