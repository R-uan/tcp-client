#pragma once

#include <cstdint>
#include <shared_mutex>
#include <sys/types.h>

#include "network/tcp_socket.h"
#ifdef _WIN32
#define API_EXPORT __declspec(dllexport)
#else
#define API_EXPORT __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

    API_EXPORT void free_ptr(const uint8_t *ptr);
    API_EXPORT int start_connection(const char *addr, const int port, const char *match_id);
        // Retrieve packet data.
    API_EXPORT uint8_t *retrieve_error(int *outSize);
    API_EXPORT uint8_t *retrieve_gamestate(int *outSize);
        // Communication through socket.
    API_EXPORT int play_card(const uint8_t *payload, int length);
    API_EXPORT int connect_player(const char *playerId, const char *playerDeckId, const char *token);

#ifdef __cplusplus
}
#endif
