#include <string>
#include <iostream>

#include "game/player.hpp"

Player::Player(std::string id, std::string token, std::string current_deck_id, std::string color)
    : id(id), token(token), current_deck_id(current_deck_id), player_color(color) {}
