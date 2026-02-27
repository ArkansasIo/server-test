#include "static.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char* guess_type(const char *path) {
  const char *dot = strrchr(path, '.');
  if (!dot) return "application/octet-stream";
  if (strcmp(dot, ".html") == 0) return "text/html; charset=utf-8";
  if (strcmp(dot, ".js") == 0) return "text/javascript; charset=utf-8";
  if (strcmp(dot, ".css") == 0) return "text/css; charset=utf-8";
  if (strcmp(dot, ".json") == 0) return "application/json; charset=utf-8";
  if (strcmp(dot, ".png") == 0) return "image/png";
  if (strcmp(dot, ".jpg") == 0 || strcmp(dot, ".jpeg") == 0) return "image/jpeg";
  if (strcmp(dot, ".svg") == 0) return "image/svg+xml";
  return "application/octet-stream";
}

static char* slurp(const char *path, size_t *out_len) {
  FILE *f = fopen(path, "rb");
  if (!f) return NULL;
  fseek(f, 0, SEEK_END);
  long n = ftell(f);
  fseek(f, 0, SEEK_SET);
  if (n < 0) { fclose(f); return NULL; }
  char *buf = (char*)malloc((size_t)n);
  if (!buf) { fclose(f); return NULL; }
  if (fread(buf, 1, (size_t)n, f) != (size_t)n) { fclose(f); free(buf); return NULL; }
  fclose(f);
  *out_len = (size_t)n;
  return buf;
}

static int is_safe_path(const char *p) {
  // Avoid .. traversal
  return (strstr(p, "..") == NULL);
}

int static_try_serve(const char *public_dir, const http_request_t *req, http_response_t *res) {
  if (strcmp(req->method, "GET") != 0) return 0;

  const char *path = req->path;
  if (strcmp(path, "/") == 0) path = "/index.html";

  if (!is_safe_path(path)) return 0;

  char full[2048];
  snprintf(full, sizeof(full), "%s%s", public_dir, path);

  size_t len = 0;
  char *data = slurp(full, &len);
  if (!data) return 0;

  http_set_body(res, guess_type(full), data, len);
  free(data);
  res->status = 200;
  return 1;
}
