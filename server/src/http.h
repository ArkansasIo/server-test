#pragma once
#include <stddef.h>

typedef struct {
  int client_fd;
  char method[8];
  char path[1024];
  char http_version[16];
  char remote_addr[64];
  char *body;      // heap allocated
  size_t body_len;
} http_request_t;

typedef struct {
  int status;
  const char *content_type;
  char *body;      // heap allocated
  size_t body_len;
} http_response_t;

int http_read_request(int client_fd, http_request_t *req, char *remote_addr, size_t remote_addr_sz);
void http_free_request(http_request_t *req);

void http_init_response(http_response_t *res);
void http_set_body(http_response_t *res, const char *content_type, const char *data, size_t len);
void http_set_body_cstr(http_response_t *res, const char *content_type, const char *cstr);
void http_send_response(int client_fd, const http_response_t *res);
void http_free_response(http_response_t *res);

const char* http_status_text(int status);
