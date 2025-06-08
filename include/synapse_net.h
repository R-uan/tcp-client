#pragma once

#include <memory>

#ifdef _WIN32
#define API_EXPORT __declspec(dllexport)
#else
#define API_EXPORT __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

    API_EXPORT void free_ptr(const uint8_t *ptr);
    API_EXPORT int start_connection(const char *addr, int port, const char *match_id);

    API_EXPORT uint8_t *retrieve_error(int *outSize);
    API_EXPORT uint8_t *retrieve_game_state(int *outSize);

    API_EXPORT int play_card(const char *card_id, const char *actor_id, const char *target_id, const char *target_position);
    API_EXPORT int connect_player(const char *player_id, const char *player_deck_id, const char *auth_token);
    API_EXPORT int reconnect_player(const char *player_id, const char *auth_token);

#ifdef __cplusplus
}
#endif
