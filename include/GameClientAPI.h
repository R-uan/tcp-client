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

    API_EXPORT uint8_t *get_next_gamestate(int *outSize);
    API_EXPORT void start_connection(const char *addr, int port, const char *match_id);
    API_EXPORT void connect_player(const char *playerId, const char *playerDeckId, const char *token);

#ifdef __cplusplus
}
#endif
