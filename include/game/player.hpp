#include <string>
#pragma

struct Player
{
    std::string id;
    std::string username;
    int level;
    std::string current_deck_id;

    Player(std::string id, std::string username, int level, std::string current_deck_id);
};