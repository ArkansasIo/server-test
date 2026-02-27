#define _POSIX_C_SOURCE 200112L
#include "api_client.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifdef _WIN32
#define close_socket closesocket
#define sock_read(fd, buf, len) recv((SOCKET)(fd), (char*)(buf), (int)(len), 0)
#define sock_write(fd, buf, len) send((SOCKET)(fd), (const char*)(buf), (int)(len), 0)
#else
#define close_socket close
#define sock_read(fd, buf, len) read((fd), (buf), (len))
#define sock_write(fd, buf, len) write((fd), (buf), (len))
#endif

static int init_network(void) {
#ifdef _WIN32
  WSADATA wsa;
  if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return -1;
#endif
  return 0;
}

static void shutdown_network(void) {
#ifdef _WIN32
  WSACleanup();
#endif
}

void api_client_init(api_client_t *c, const char *host, int port) {
  memset(c, 0, sizeof(*c));
  snprintf(c->host, sizeof(c->host), "%s", host ? host : "127.0.0.1");
  c->port = port > 0 ? port : 8080;
}

static int http_get(const api_client_t *c, const char *path, char *out, size_t out_sz) {
  if (init_network() != 0) return -1;

  int fd = (int)socket(AF_INET, SOCK_STREAM, 0);
#ifdef _WIN32
  if ((SOCKET)fd == INVALID_SOCKET) { shutdown_network(); return -1; }
#else
  if (fd < 0) return -1;
#endif

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons((uint16_t)c->port);
  if (inet_pton(AF_INET, c->host, &addr.sin_addr) != 1) { close_socket(fd); shutdown_network(); return -1; }

  if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) != 0) { close_socket(fd); shutdown_network(); return -1; }

  char req[512];
  int rn = snprintf(req, sizeof(req),
    "GET %s HTTP/1.1\r\n"
    "Host: %s:%d\r\n"
    "Connection: close\r\n"
    "\r\n",
    path, c->host, c->port
  );
  if (rn <= 0) { close_socket(fd); shutdown_network(); return -1; }
  (void)sock_write(fd, req, (size_t)rn);

  // read all into temp
  size_t cap = 65536;
  char *buf = (char*)calloc(1, cap);
  if (!buf) { close_socket(fd); shutdown_network(); return -1; }
  size_t n = 0;
  while (n + 1 < cap) {
    int r = sock_read(fd, buf + n, cap - n - 1);
    if (r <= 0) break;
    n += (size_t)r;
  }
  buf[n] = 0;
  close_socket(fd);
  shutdown_network();

  // find body
  char *body = strstr(buf, "\r\n\r\n");
  if (!body) { free(buf); return -1; }
  body += 4;

  // copy to out
  snprintf(out, out_sz, "%s", body);
  free(buf);
  return 0;
}

int api_get_health(const api_client_t *c, char *out_buf, size_t out_sz) {
  return http_get(c, "/api/health", out_buf, out_sz);
}

int api_get_items(const api_client_t *c, char *out_buf, size_t out_sz) {
  return http_get(c, "/api/items", out_buf, out_sz);
}
