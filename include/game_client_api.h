#pragma once

#include <cstdint>
#ifdef _WIN32
#define API_EXPORT __declspec(dllexport)
#else
#define API_EXPORT __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    API_EXPORT void start_connection(const char *addr, int port, const char *match_id);
    API_EXPORT ssize_t connect_player(const char *playerId, const char *playerDeckId, const char *token);

    API_EXPORT void free_ptr(const uint8_t *ptr);

    API_EXPORT uint8_t *retrieve_error(int *outSize);
    API_EXPORT uint8_t *retrieve_gamestate(int *outSize);

    API_EXPORT ssize_t play_card(const char *payload, int length);

#ifdef __cplusplus
}
#endif
