#pragma once
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  char host[256];   // e.g. "127.0.0.1"
  int port;         // e.g. 8080
} api_client_t;

void api_client_init(api_client_t *c, const char *host, int port);

// Returns 0 on success; response JSON is placed in out_buf (null-terminated).
int api_get_health(const api_client_t *c, char *out_buf, size_t out_sz);

// Returns 0 on success; response JSON is placed in out_buf.
int api_get_items(const api_client_t *c, char *out_buf, size_t out_sz);

#ifdef __cplusplus
}
#endif
