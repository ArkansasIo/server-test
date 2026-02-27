#include "http.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#define sock_read(fd, buf, len) recv((SOCKET)(fd), (char*)(buf), (int)(len), 0)
#define sock_write(fd, buf, len) send((SOCKET)(fd), (const char*)(buf), (int)(len), 0)
#else
#include <unistd.h>
#include <strings.h>
#define sock_read(fd, buf, len) read((fd), (buf), (len))
#define sock_write(fd, buf, len) write((fd), (buf), (len))
#endif

static const char* strcasestr_local(const char *haystack, const char *needle) {
  if (!haystack || !needle || !*needle) return haystack;
  size_t nl = strlen(needle);
  for (const char *p = haystack; *p; p++) {
#ifdef _WIN32
    if (_strnicmp(p, needle, nl) == 0) return p;
#else
    if (strncasecmp(p, needle, nl) == 0) return p;
#endif
  }
  return NULL;
}

static int read_until(int fd, char *buf, size_t cap, const char *needle) {
  size_t n = 0;
  while (n + 1 < cap) {
    int r = sock_read(fd, buf + n, 1);
    if (r <= 0) return -1;
    n += (size_t)r;
    buf[n] = 0;
    if (strstr(buf, needle) != NULL) return (int)n;
  }
  return -1;
}

int http_read_request(int client_fd, http_request_t *req, char *remote_addr, size_t remote_addr_sz) {
  memset(req, 0, sizeof(*req));
  snprintf(req->remote_addr, sizeof(req->remote_addr), "%s", remote_addr ? remote_addr : "unknown");

  // Read headers (very small/simple server: cap 64KB)
  size_t cap = 65536;
  char *hdr = (char*)calloc(1, cap);
  if (!hdr) return -1;

  int hdr_len = read_until(client_fd, hdr, cap, "\r\n\r\n");
  if (hdr_len < 0) { free(hdr); return -1; }

  // Parse request line
  // Example: GET /path HTTP/1.1
  char *line_end = strstr(hdr, "\r\n");
  if (!line_end) { free(hdr); return -1; }
  *line_end = 0;

  if (sscanf(hdr, "%7s %1023s %15s", req->method, req->path, req->http_version) != 3) {
    free(hdr); return -1;
  }

  // Content-Length
  size_t content_length = 0;
  const char *cl = strcasestr_local(line_end + 2, "Content-Length:");
  if (cl) {
    cl += strlen("Content-Length:");
    while (*cl == ' ' || *cl == '\t') cl++;
    content_length = (size_t)strtoul(cl, NULL, 10);
  }

  // If body exists, read it
  req->body = NULL;
  req->body_len = 0;

  char *body_start = strstr(line_end + 1, "\r\n\r\n");
  if (!body_start) { free(hdr); return 0; }
  body_start += 4;

  size_t already = (size_t)(hdr + hdr_len - body_start);
  if (content_length > 0) {
    req->body = (char*)calloc(1, content_length + 1);
    if (!req->body) { free(hdr); return -1; }

    // copy what we already have after headers
    size_t copy = already > content_length ? content_length : already;
    memcpy(req->body, body_start, copy);
    req->body_len = copy;

    while (req->body_len < content_length) {
      int r = sock_read(client_fd, req->body + req->body_len, content_length - req->body_len);
      if (r <= 0) break;
      req->body_len += (size_t)r;
    }
    req->body[req->body_len] = 0;
  }

  free(hdr);
  return 0;
}

void http_free_request(http_request_t *req) {
  if (req && req->body) free(req->body);
  if (req) memset(req, 0, sizeof(*req));
}

void http_init_response(http_response_t *res) {
  memset(res, 0, sizeof(*res));
  res->status = 200;
  res->content_type = "text/plain; charset=utf-8";
  res->body = NULL;
  res->body_len = 0;
}

void http_set_body(http_response_t *res, const char *content_type, const char *data, size_t len) {
  if (res->body) free(res->body);
  res->content_type = content_type ? content_type : "application/octet-stream";
  res->body = (char*)malloc(len);
  if (!res->body) { res->body_len = 0; return; }
  memcpy(res->body, data, len);
  res->body_len = len;
}

void http_set_body_cstr(http_response_t *res, const char *content_type, const char *cstr) {
  http_set_body(res, content_type, cstr, strlen(cstr));
}

const char* http_status_text(int status) {
  switch (status) {
    case 200: return "OK";
    case 201: return "Created";
    case 400: return "Bad Request";
    case 404: return "Not Found";
    case 405: return "Method Not Allowed";
    case 500: return "Internal Server Error";
    default: return "OK";
  }
}

void http_send_response(int client_fd, const http_response_t *res) {
  char header[2048];
  int n = snprintf(header, sizeof(header),
    "HTTP/1.1 %d %s\r\n"
    "Content-Type: %s\r\n"
    "Content-Length: %zu\r\n"
    "Connection: close\r\n"
    "Access-Control-Allow-Origin: *\r\n"
    "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
    "Access-Control-Allow-Headers: Content-Type\r\n"
    "\r\n",
    res->status, http_status_text(res->status),
    res->content_type ? res->content_type : "application/octet-stream",
    res->body_len
  );
  if (n > 0) (void)sock_write(client_fd, header, (size_t)n);
  if (res->body && res->body_len) (void)sock_write(client_fd, res->body, res->body_len);
}

void http_free_response(http_response_t *res) {
  if (res && res->body) free(res->body);
  if (res) memset(res, 0, sizeof(*res));
}
