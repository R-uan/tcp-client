#include <string>
#include <iostream>

#include "game/player.hpp"

Player::Player(std::string id, std::string username, int level, std::string current_deck_id)
    : id(id), username(username), level(level), current_deck_id(current_deck_id) {}
