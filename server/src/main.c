#define _POSIX_C_SOURCE 200112L
#include "http.h"
#include "db.h"
#include "static.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

static volatile int g_running = 1;

#ifdef _WIN32
typedef SOCKET socket_t;
#define close_socket closesocket
#else
typedef int socket_t;
#define close_socket close
#endif

typedef struct {
  int port;
  char public_dir[512];
  char db_path[512];
  char schema_path[512];
  char seed_path[512];
  int seed_enabled;
  db_options_t db_options;
  char db_journal_mode[32];
} app_config_t;

static void on_sigint(int sig) {
  (void)sig;
  g_running = 0;
}

static char* trim(char *s) {
  while (*s && isspace((unsigned char)*s)) s++;
  char *end = s + strlen(s);
  while (end > s && isspace((unsigned char)end[-1])) end--;
  *end = '\0';
  return s;
}

static int parse_bool(const char *s, int default_value) {
  if (!s || !*s) return default_value;
  if (strcmp(s, "1") == 0 || strcmp(s, "true") == 0 || strcmp(s, "TRUE") == 0 || strcmp(s, "on") == 0 || strcmp(s, "ON") == 0 || strcmp(s, "yes") == 0 || strcmp(s, "YES") == 0) return 1;
  if (strcmp(s, "0") == 0 || strcmp(s, "false") == 0 || strcmp(s, "FALSE") == 0 || strcmp(s, "off") == 0 || strcmp(s, "OFF") == 0 || strcmp(s, "no") == 0 || strcmp(s, "NO") == 0) return 0;
  return default_value;
}

static int ensure_data_dir(void) {
#ifdef _WIN32
  if (_mkdir("data") == 0 || errno == EEXIST) return 0;
#else
  if (mkdir("data", 0755) == 0 || errno == EEXIST) return 0;
#endif
  return -1;
}

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

static void config_defaults(app_config_t *cfg) {
  memset(cfg, 0, sizeof(*cfg));
  cfg->port = 8080;
  snprintf(cfg->public_dir, sizeof(cfg->public_dir), "./server/public");
  snprintf(cfg->db_path, sizeof(cfg->db_path), "./data/app.db");
  snprintf(cfg->schema_path, sizeof(cfg->schema_path), "./db/schema.sql");
  snprintf(cfg->seed_path, sizeof(cfg->seed_path), "./db/seed.sql");
  cfg->seed_enabled = 1;
  snprintf(cfg->db_journal_mode, sizeof(cfg->db_journal_mode), "WAL");
  cfg->db_options.journal_mode = cfg->db_journal_mode;
  cfg->db_options.foreign_keys = 1;
  cfg->db_options.busy_timeout_ms = 5000;
}

static void config_apply_kv(app_config_t *cfg, const char *key, const char *value) {
  if (!key || !value) return;

  if (strcmp(key, "PORT") == 0) cfg->port = atoi(value);
  else if (strcmp(key, "PUBLIC_DIR") == 0) snprintf(cfg->public_dir, sizeof(cfg->public_dir), "%s", value);
  else if (strcmp(key, "DB_PATH") == 0) snprintf(cfg->db_path, sizeof(cfg->db_path), "%s", value);
  else if (strcmp(key, "DB_SCHEMA") == 0) snprintf(cfg->schema_path, sizeof(cfg->schema_path), "%s", value);
  else if (strcmp(key, "DB_SEED_FILE") == 0) snprintf(cfg->seed_path, sizeof(cfg->seed_path), "%s", value);
  else if (strcmp(key, "DB_ENABLE_SEED") == 0) cfg->seed_enabled = parse_bool(value, cfg->seed_enabled);
  else if (strcmp(key, "DB_JOURNAL_MODE") == 0) {
    snprintf(cfg->db_journal_mode, sizeof(cfg->db_journal_mode), "%s", value);
    cfg->db_options.journal_mode = cfg->db_journal_mode;
  }
  else if (strcmp(key, "DB_FOREIGN_KEYS") == 0) cfg->db_options.foreign_keys = parse_bool(value, cfg->db_options.foreign_keys);
  else if (strcmp(key, "DB_BUSY_TIMEOUT_MS") == 0) cfg->db_options.busy_timeout_ms = atoi(value);
}

static int config_load_file(app_config_t *cfg, const char *path) {
  FILE *f = fopen(path, "rb");
  if (!f) return -1;

  char line[1024];
  while (fgets(line, sizeof(line), f)) {
    char *s = trim(line);
    if (*s == '\0' || *s == '#') continue;

    char *eq = strchr(s, '=');
    if (!eq) continue;
    *eq = '\0';

    char *key = trim(s);
    char *value = trim(eq + 1);
    config_apply_kv(cfg, key, value);
  }

  fclose(f);
  return 0;
}

static void config_apply_env(app_config_t *cfg) {
  const char *v = NULL;

  v = getenv("APP_PORT"); if (v) cfg->port = atoi(v);
  v = getenv("APP_PUBLIC_DIR"); if (v) snprintf(cfg->public_dir, sizeof(cfg->public_dir), "%s", v);

  v = getenv("APP_DB_PATH"); if (v) snprintf(cfg->db_path, sizeof(cfg->db_path), "%s", v);
  v = getenv("APP_DB_SCHEMA"); if (v) snprintf(cfg->schema_path, sizeof(cfg->schema_path), "%s", v);
  v = getenv("APP_DB_SEED_FILE"); if (v) snprintf(cfg->seed_path, sizeof(cfg->seed_path), "%s", v);
  v = getenv("APP_DB_ENABLE_SEED"); if (v) cfg->seed_enabled = parse_bool(v, cfg->seed_enabled);
  v = getenv("APP_DB_JOURNAL_MODE"); if (v) { snprintf(cfg->db_journal_mode, sizeof(cfg->db_journal_mode), "%s", v); cfg->db_options.journal_mode = cfg->db_journal_mode; }
  v = getenv("APP_DB_FOREIGN_KEYS"); if (v) cfg->db_options.foreign_keys = parse_bool(v, cfg->db_options.foreign_keys);
  v = getenv("APP_DB_BUSY_TIMEOUT_MS"); if (v) cfg->db_options.busy_timeout_ms = atoi(v);
}

static void print_usage(const char *prog) {
  printf("Usage: %s [options]\n", prog);
  printf("  --port <n>                HTTP port (default 8080)\n");
  printf("  --public <dir>            Static files dir\n");
  printf("  --db <path>               SQLite DB path\n");
  printf("  --db-schema <path>        Schema SQL file\n");
  printf("  --db-seed <path>          Seed SQL file\n");
  printf("  --db-seed-enable <0|1>    Enable/disable seeding\n");
  printf("  --db-journal <mode>       SQLite journal mode (WAL/DELETE/TRUNCATE/MEMORY/OFF)\n");
  printf("  --db-foreign-keys <0|1>   PRAGMA foreign_keys\n");
  printf("  --db-timeout-ms <n>       sqlite busy timeout\n");
  printf("  --db-config <path>        Key=Value config file\n");
  printf("  --help                    Show this help\n");
}

static int log_api(app_db_t *adb, const http_request_t *req, int status) {
  const char *sql = "INSERT INTO api_logs (method, path, status_code, remote_addr) VALUES (?, ?, ?, ?);";
  sqlite3_stmt *st = NULL;
  if (sqlite3_prepare_v2(adb->db, sql, -1, &st, NULL) != SQLITE_OK) return -1;
  sqlite3_bind_text(st, 1, req->method, -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(st, 2, req->path, -1, SQLITE_TRANSIENT);
  sqlite3_bind_int(st, 3, status);
  sqlite3_bind_text(st, 4, req->remote_addr, -1, SQLITE_TRANSIENT);
  int rc = sqlite3_step(st);
  sqlite3_finalize(st);
  return (rc == SQLITE_DONE) ? 0 : -1;
}

static void route_api(app_db_t *adb, const http_request_t *req, http_response_t *res) {
  if (strcmp(req->method, "OPTIONS") == 0) {
    res->status = 200;
    http_set_body_cstr(res, "text/plain; charset=utf-8", "ok");
    return;
  }

  if (strcmp(req->path, "/api/health") == 0) {
    if (strcmp(req->method, "GET") != 0) { res->status = 405; http_set_body_cstr(res, "application/json; charset=utf-8", "{ \"error\": \"method_not_allowed\" }"); return; }
    char *j = NULL;
    if (db_health(adb, &j) != 0 || !j) { res->status = 500; http_set_body_cstr(res, "application/json; charset=utf-8", "{ \"error\": \"db\" }"); return; }
    res->status = 200;
    http_set_body_cstr(res, "application/json; charset=utf-8", j);
    free(j);
    return;
  }

  if (strcmp(req->path, "/api/items") == 0) {
    if (strcmp(req->method, "GET") == 0) {
      char *j = NULL;
      if (db_list_items(adb, &j) != 0 || !j) { res->status = 500; http_set_body_cstr(res, "application/json; charset=utf-8", "{ \"error\": \"db\" }"); return; }
      res->status = 200;
      http_set_body(res, "application/json; charset=utf-8", j, strlen(j));
      free(j);
      return;
    }
    if (strcmp(req->method, "POST") == 0) {
      char *j = NULL;
      if (db_create_item(adb, req->body, &j) != 0 || !j) { res->status = 400; http_set_body_cstr(res, "application/json; charset=utf-8", "{ \"error\": \"bad_request\" }"); return; }
      res->status = 201;
      http_set_body_cstr(res, "application/json; charset=utf-8", j);
      free(j);
      return;
    }
    res->status = 405;
    http_set_body_cstr(res, "application/json; charset=utf-8", "{ \"error\": \"method_not_allowed\" }");
    return;
  }

  /* Authentication endpoints */
  if (strcmp(req->path, "/api/auth/login") == 0) {
    if (strcmp(req->method, "POST") != 0) { res->status = 405; http_set_body_cstr(res, "application/json; charset=utf-8", "{ \"error\": \"method_not_allowed\" }"); return; }
    
    char email[256] = {0};
    char password[256] = {0};
    
    /* Simple JSON parsing - extract email and password fields */
    const char *email_start = strstr(req->body, "\"email\":");
    const char *pass_start = strstr(req->body, "\"password\":");
    
    if (email_start && pass_start) {
      const char *p = strchr(email_start, '"');
      if (p) {
        p++;
        int i = 0;
        while (*p && *p != '"' && i < (int)sizeof(email) - 1) {
          email[i++] = *p++;
        }
        email[i] = '\0';
      }
      
      p = strchr(pass_start, '"');
      if (p) {
        p++;
        int i = 0;
        while (*p && *p != '"' && i < (int)sizeof(password) - 1) {
          password[i++] = *p++;
        }
        password[i] = '\0';
      }
    }
    
    char *j = NULL;
    if (db_login(adb, email, password, &j) != 0 || !j) { 
      res->status = 401; 
      http_set_body_cstr(res, "application/json; charset=utf-8", "{ \"ok\": false, \"error\": \"Unauthorized\" }"); 
      return; 
    }
    res->status = 200;
    http_set_body_cstr(res, "application/json; charset=utf-8", j);
    free(j);
    return;
  }

  if (strcmp(req->path, "/api/auth/register") == 0) {
    if (strcmp(req->method, "POST") != 0) { res->status = 405; http_set_body_cstr(res, "application/json; charset=utf-8", "{ \"error\": \"method_not_allowed\" }"); return; }
    
    char email[256] = {0};
    char password[256] = {0};
    char display_name[256] = {0};
    
    /* Simple JSON parsing */
    const char *p = NULL;
    
    p = strstr(req->body, "\"email\":");
    if (p) {
      p = strchr(p, '"');
      if (p) {
        p++;
        int i = 0;
        while (*p && *p != '"' && i < (int)sizeof(email) - 1) {
          email[i++] = *p++;
        }
        email[i] = '\0';
      }
    }
    
    p = strstr(req->body, "\"password\":");
    if (p) {
      p = strchr(p, '"');
      if (p) {
        p++;
        int i = 0;
        while (*p && *p != '"' && i < (int)sizeof(password) - 1) {
          password[i++] = *p++;
        }
        password[i] = '\0';
      }
    }
    
    p = strstr(req->body, "\"display_name\":");
    if (p) {
      p = strchr(p, '"');
      if (p) {
        p++;
        int i = 0;
        while (*p && *p != '"' && i < (int)sizeof(display_name) - 1) {
          display_name[i++] = *p++;
        }
        display_name[i] = '\0';
      }
    }
    
    char *j = NULL;
    if (db_register(adb, email, password, display_name, &j) != 0 || !j) {
      res->status = 400;
      http_set_body_cstr(res, "application/json; charset=utf-8", "{ \"ok\": false, \"error\": \"Registration failed\" }");
      return;
    }
    res->status = 201;
    http_set_body_cstr(res, "application/json; charset=utf-8", j);
    free(j);
    return;
  }

  if (strcmp(req->path, "/api/users") == 0) {
    if (strcmp(req->method, "GET") == 0) {
      char *j = NULL;
      if (db_list_users(adb, &j) != 0 || !j) { res->status = 500; http_set_body_cstr(res, "application/json; charset=utf-8", "{ \"error\": \"db\" }"); return; }
      res->status = 200;
      http_set_body(res, "application/json; charset=utf-8", j, strlen(j));
      free(j);
      return;
    }
    res->status = 405;
    http_set_body_cstr(res, "application/json; charset=utf-8", "{ \"error\": \"method_not_allowed\" }");
    return;
  }

  res->status = 404;
  http_set_body_cstr(res, "application/json; charset=utf-8", "{ \"error\": \"not_found\" }");
}

static int start_listen(int port) {
  socket_t fd = socket(AF_INET, SOCK_STREAM, 0);
#ifdef _WIN32
  if (fd == INVALID_SOCKET) return -1;
#else
  if (fd < 0) return -1;
#endif

  int yes = 1;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&yes, sizeof(yes));

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons((uint16_t)port);

  if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) != 0) { close_socket(fd); return -1; }
  if (listen(fd, 64) != 0) { close_socket(fd); return -1; }

  return (int)fd;
}

int main(int argc, char **argv) {
  app_config_t cfg;
  config_defaults(&cfg);

  const char *db_config_path = "./server/db.conf";
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--db-config") == 0 && i + 1 < argc) {
      db_config_path = argv[++i];
    }
  }

  (void)config_load_file(&cfg, db_config_path);
  config_apply_env(&cfg);

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--port") == 0 && i + 1 < argc) cfg.port = atoi(argv[++i]);
    else if (strcmp(argv[i], "--db") == 0 && i + 1 < argc) snprintf(cfg.db_path, sizeof(cfg.db_path), "%s", argv[++i]);
    else if (strcmp(argv[i], "--public") == 0 && i + 1 < argc) snprintf(cfg.public_dir, sizeof(cfg.public_dir), "%s", argv[++i]);
    else if (strcmp(argv[i], "--db-schema") == 0 && i + 1 < argc) snprintf(cfg.schema_path, sizeof(cfg.schema_path), "%s", argv[++i]);
    else if (strcmp(argv[i], "--db-seed") == 0 && i + 1 < argc) snprintf(cfg.seed_path, sizeof(cfg.seed_path), "%s", argv[++i]);
    else if (strcmp(argv[i], "--db-seed-enable") == 0 && i + 1 < argc) cfg.seed_enabled = parse_bool(argv[++i], cfg.seed_enabled);
    else if (strcmp(argv[i], "--db-journal") == 0 && i + 1 < argc) { snprintf(cfg.db_journal_mode, sizeof(cfg.db_journal_mode), "%s", argv[++i]); cfg.db_options.journal_mode = cfg.db_journal_mode; }
    else if (strcmp(argv[i], "--db-foreign-keys") == 0 && i + 1 < argc) cfg.db_options.foreign_keys = parse_bool(argv[++i], cfg.db_options.foreign_keys);
    else if (strcmp(argv[i], "--db-timeout-ms") == 0 && i + 1 < argc) cfg.db_options.busy_timeout_ms = atoi(argv[++i]);
    else if (strcmp(argv[i], "--db-config") == 0 && i + 1 < argc) { i++; }
    else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) { print_usage(argv[0]); return 0; }
  }

  signal(SIGINT, on_sigint);
  signal(SIGTERM, on_sigint);

  if (ensure_data_dir() != 0) {
    fprintf(stderr, "Failed to create data directory\n");
    return 1;
  }

  if (init_network() != 0) {
    fprintf(stderr, "Failed to initialize network\n");
    return 1;
  }

  app_db_t adb;
  if (db_open(&adb, cfg.db_path, &cfg.db_options) != 0) {
    fprintf(stderr, "DB open failed\n");
    shutdown_network();
    return 1;
  }
  if (db_migrate(&adb, cfg.schema_path) != 0) {
    fprintf(stderr, "DB migrate failed\n");
    db_close(&adb);
    shutdown_network();
    return 1;
  }
  if (cfg.seed_enabled) {
    (void)db_seed(&adb, cfg.seed_path);
  }

  int server_fd = start_listen(cfg.port);
  if (server_fd < 0) {
    fprintf(stderr, "Failed to listen on port %d\n", cfg.port);
    db_close(&adb);
    shutdown_network();
    return 1;
  }

  printf("Server running: http://localhost:%d/\n", cfg.port);
  printf("API: http://localhost:%d/api/health\n", cfg.port);
  printf("DB: path=%s, journal=%s, foreign_keys=%d, timeout_ms=%d, seed=%d\n",
         cfg.db_path,
         cfg.db_options.journal_mode ? cfg.db_options.journal_mode : "",
         cfg.db_options.foreign_keys,
         cfg.db_options.busy_timeout_ms,
         cfg.seed_enabled);

  while (g_running) {
    struct sockaddr_in caddr;
    #ifdef _WIN32
    int clen = (int)sizeof(caddr);
    SOCKET cfd = accept((SOCKET)server_fd, (struct sockaddr*)&caddr, &clen);
    if (cfd == INVALID_SOCKET) {
      if (WSAGetLastError() == WSAEINTR) continue;
      break;
    }
    #else
    socklen_t clen = sizeof(caddr);
    int cfd = accept(server_fd, (struct sockaddr*)&caddr, &clen);
    if (cfd < 0) {
      if (errno == EINTR) continue;
      break;
    }
    #endif

    char rip[64] = {0};
    inet_ntop(AF_INET, &caddr.sin_addr, rip, sizeof(rip));

    http_request_t req;
    if (http_read_request(cfd, &req, rip, sizeof(rip)) != 0) {
      close_socket(cfd);
      continue;
    }

    http_response_t res;
    http_init_response(&res);

    int served = 0;
    if (strncmp(req.path, "/api", 4) != 0) {
      served = static_try_serve(cfg.public_dir, &req, &res);
      if (!served && strcmp(req.path, "/") == 0) served = static_try_serve(cfg.public_dir, &req, &res);
    }

    if (!served) {
      route_api(&adb, &req, &res);
      (void)log_api(&adb, &req, res.status);
    }

    http_send_response(cfd, &res);

    http_free_response(&res);
    http_free_request(&req);
    close_socket(cfd);
  }

  close_socket(server_fd);
  db_close(&adb);
  shutdown_network();
  printf("Server stopped.\n");
  return 0;
}
