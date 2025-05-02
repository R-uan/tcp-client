#include <string>
#pragma

struct Player
{
    std::string id;
    std::string token;
    std::string current_deck_id;
    std::string player_color;

    Player(std::string id, std::string token, std::string current_deck_id, std::string color);
};