#pragma once

#ifdef _WIN32
#define API_EXPORT __declspec(dllexport)
#else
#define API_EXPORT __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    API_EXPORT void start_connection(const char *addr, int port, const char *token, const char *match_id);
    API_EXPORT void connect_player();

#ifdef __cplusplus
}
#endif
