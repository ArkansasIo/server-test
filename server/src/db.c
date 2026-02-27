#include "db.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char* slurp_file(const char *path) {
  FILE *f = fopen(path, "rb");
  if (!f) return NULL;
  fseek(f, 0, SEEK_END);
  long n = ftell(f);
  fseek(f, 0, SEEK_SET);
  if (n < 0) { fclose(f); return NULL; }
  char *buf = (char*)malloc((size_t)n + 1);
  if (!buf) { fclose(f); return NULL; }
  if (fread(buf, 1, (size_t)n, f) != (size_t)n) { fclose(f); free(buf); return NULL; }
  buf[n] = 0;
  fclose(f);
  return buf;
}

static int exec_sql(sqlite3 *db, const char *sql) {
  char *err = NULL;
  int rc = sqlite3_exec(db, sql, NULL, NULL, &err);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "sqlite exec error: %s\n", err ? err : "(unknown)");
    sqlite3_free(err);
    return -1;
  }
  return 0;
}

int db_open(app_db_t *adb, const char *path, const db_options_t *opts) {
  memset(adb, 0, sizeof(*adb));
  if (sqlite3_open(path, &adb->db) != SQLITE_OK) {
    fprintf(stderr, "sqlite open failed: %s\n", sqlite3_errmsg(adb->db));
    return -1;
  }

  if (opts) {
    if (opts->busy_timeout_ms > 0) {
      sqlite3_busy_timeout(adb->db, opts->busy_timeout_ms);
    }

    if (opts->journal_mode && opts->journal_mode[0]) {
      char *sql = sqlite3_mprintf("PRAGMA journal_mode=%q;", opts->journal_mode);
      if (sql) {
        (void)exec_sql(adb->db, sql);
        sqlite3_free(sql);
      }
    }

    (void)exec_sql(adb->db, opts->foreign_keys ? "PRAGMA foreign_keys=ON;" : "PRAGMA foreign_keys=OFF;");
  } else {
    (void)exec_sql(adb->db, "PRAGMA journal_mode=WAL;");
    (void)exec_sql(adb->db, "PRAGMA foreign_keys=ON;");
    sqlite3_busy_timeout(adb->db, 5000);
  }

  return 0;
}

void db_close(app_db_t *adb) {
  if (adb && adb->db) sqlite3_close(adb->db);
  if (adb) memset(adb, 0, sizeof(*adb));
}

int db_migrate(app_db_t *adb, const char *schema_sql_path) {
  char *sql = slurp_file(schema_sql_path);
  if (!sql) return -1;
  int rc = exec_sql(adb->db, sql);
  free(sql);
  return rc;
}

int db_seed(app_db_t *adb, const char *seed_sql_path) {
  char *sql = slurp_file(seed_sql_path);
  if (!sql) return -1;
  int rc = exec_sql(adb->db, sql);
  free(sql);
  return rc;
}

static char* json_escape(const char *s) {
  if (!s) s = "";
  size_t cap = strlen(s) * 2 + 32;
  char *out = (char*)malloc(cap);
  if (!out) return NULL;
  size_t j = 0;
  out[j++] = '"';
  for (size_t i = 0; s[i]; i++) {
    char c = s[i];
    const char *rep = NULL;
    char tmp[7] = {0};
    if (c == '\\') rep = "\\\\";
    else if (c == '"') rep = "\\\"";
    else if (c == '\n') rep = "\\n";
    else if (c == '\r') rep = "\\r";
    else if (c == '\t') rep = "\\t";
    else if ((unsigned char)c < 0x20) { snprintf(tmp, sizeof(tmp), "\\u%04x", (unsigned char)c); rep = tmp; }
    if (rep) {
      size_t rl = strlen(rep);
      if (j + rl + 2 > cap) { cap *= 2; out = (char*)realloc(out, cap); if (!out) return NULL; }
      memcpy(out + j, rep, rl); j += rl;
    } else {
      if (j + 2 > cap) { cap *= 2; out = (char*)realloc(out, cap); if (!out) return NULL; }
      out[j++] = c;
    }
  }
  out[j++] = '"';
  out[j] = 0;
  return out;
}

static int parse_int_field(const char *json, const char *key, int *out) {
  const char *p = strstr(json, key);
  if (!p) return -1;
  p = strchr(p, ':');
  if (!p) return -1;
  p++;
  while (*p == ' ' || *p == '\t') p++;
  *out = (int)strtol(p, NULL, 10);
  return 0;
}

static int parse_string_field(const char *json, const char *key, char *out, size_t out_sz) {
  const char *p = strstr(json, key);
  if (!p) return -1;
  p = strchr(p, ':');
  if (!p) return -1;
  p++;
  while (*p == ' ' || *p == '\t') p++;
  if (*p != '"') return -1;
  p++;
  size_t j = 0;
  while (*p && *p != '"' && j + 1 < out_sz) {
    if (*p == '\\') {
      p++;
      if (!*p) break;
      char c = *p;
      if (c == 'n') out[j++] = '\n';
      else if (c == 'r') out[j++] = '\r';
      else if (c == 't') out[j++] = '\t';
      else out[j++] = c;
      p++;
    } else {
      out[j++] = *p++;
    }
  }
  out[j] = 0;
  return 0;
}

int db_health(app_db_t *adb, char **out_json) {
  (void)adb;
  const char *j = "{ \"ok\": true, \"service\": \"c-fullstack\", \"version\": \"1.0.0\" }";
  *out_json = strdup(j);
  return *out_json ? 0 : -1;
}

int db_list_items(app_db_t *adb, char **out_json) {
  const char *sql =
    "SELECT items.id, items.user_id, users.email, items.title, items.body, items.status, items.created_at, items.updated_at "
    "FROM items JOIN users ON users.id = items.user_id "
    "ORDER BY items.id DESC LIMIT 100;";
  sqlite3_stmt *st = NULL;
  if (sqlite3_prepare_v2(adb->db, sql, -1, &st, NULL) != SQLITE_OK) return -1;

  size_t cap = 8192;
  char *buf = (char*)malloc(cap);
  if (!buf) { sqlite3_finalize(st); return -1; }
  size_t n = 0;

  n += (size_t)snprintf(buf + n, cap - n, "{ \"items\": [");
  int first = 1;
  while (sqlite3_step(st) == SQLITE_ROW) {
    int id = sqlite3_column_int(st, 0);
    int user_id = sqlite3_column_int(st, 1);
    const char *email = (const char*)sqlite3_column_text(st, 2);
    const char *title = (const char*)sqlite3_column_text(st, 3);
    const char *body = (const char*)sqlite3_column_text(st, 4);
    const char *status = (const char*)sqlite3_column_text(st, 5);
    const char *created_at = (const char*)sqlite3_column_text(st, 6);
    const char *updated_at = (const char*)sqlite3_column_text(st, 7);

    char *j_email = json_escape(email);
    char *j_title = json_escape(title);
    char *j_body  = json_escape(body);
    char *j_status= json_escape(status);
    char *j_ca    = json_escape(created_at);
    char *j_ua    = json_escape(updated_at);
    if (!j_email || !j_title || !j_body || !j_status || !j_ca || !j_ua) { free(buf); sqlite3_finalize(st); return -1; }

    char row[4096];
    int rn = snprintf(row, sizeof(row),
      "%s{ \"id\": %d, \"user_id\": %d, \"email\": %s, \"title\": %s, \"body\": %s, \"status\": %s, \"created_at\": %s, \"updated_at\": %s }",
      first ? "" : ",", id, user_id, j_email, j_title, j_body, j_status, j_ca, j_ua
    );
    first = 0;

    if (n + (size_t)rn + 64 > cap) {
      cap *= 2;
      buf = (char*)realloc(buf, cap);
      if (!buf) { sqlite3_finalize(st); return -1; }
    }
    memcpy(buf + n, row, (size_t)rn);
    n += (size_t)rn;

    free(j_email); free(j_title); free(j_body); free(j_status); free(j_ca); free(j_ua);
  }

  sqlite3_finalize(st);
  if (n + 32 > cap) { cap += 64; buf = (char*)realloc(buf, cap); if (!buf) return -1; }
  n += (size_t)snprintf(buf + n, cap - n, "] }");
  *out_json = buf;
  return 0;
}

int db_create_item(app_db_t *adb, const char *json_body, char **out_json) {
  if (!json_body) return -1;

  int user_id = 0;
  char title[256] = {0};
  char body[2048] = {0};

  if (parse_int_field(json_body, "\"user_id\"", &user_id) != 0) return -1;
  if (parse_string_field(json_body, "\"title\"", title, sizeof(title)) != 0) return -1;
  if (parse_string_field(json_body, "\"body\"", body, sizeof(body)) != 0) return -1;

  const char *sql = "INSERT INTO items (user_id, title, body) VALUES (?, ?, ?);";
  sqlite3_stmt *st = NULL;
  if (sqlite3_prepare_v2(adb->db, sql, -1, &st, NULL) != SQLITE_OK) return -1;

  sqlite3_bind_int(st, 1, user_id);
  sqlite3_bind_text(st, 2, title, -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(st, 3, body, -1, SQLITE_TRANSIENT);

  if (sqlite3_step(st) != SQLITE_DONE) { sqlite3_finalize(st); return -1; }
  sqlite3_finalize(st);

  long long new_id = sqlite3_last_insert_rowid(adb->db);

  char resp[512];
  snprintf(resp, sizeof(resp), "{ \"ok\": true, \"id\": %lld }", new_id);
  *out_json = strdup(resp);
  return *out_json ? 0 : -1;
}
