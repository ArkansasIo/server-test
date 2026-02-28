#pragma once
#include <sqlite3.h>

typedef struct {
  const char *journal_mode;   // e.g. WAL, DELETE, TRUNCATE, MEMORY, OFF
  int foreign_keys;           // 0 or 1
  int busy_timeout_ms;        // e.g. 5000
} db_options_t;

typedef struct {
  sqlite3 *db;
} app_db_t;

int db_open(app_db_t *adb, const char *path, const db_options_t *opts);
void db_close(app_db_t *adb);

int db_migrate(app_db_t *adb, const char *schema_sql_path);
int db_seed(app_db_t *adb, const char *seed_sql_path);

int db_health(app_db_t *adb, char **out_json);
int db_list_items(app_db_t *adb, char **out_json);
int db_create_item(app_db_t *adb, const char *json_body, char **out_json);

/* Authentication functions */
int db_login(app_db_t *adb, const char *email, const char *password_hash, char **out_json);
int db_register(app_db_t *adb, const char *email, const char *password_hash, const char *display_name, char **out_json);
int db_get_user(app_db_t *adb, int user_id, char **out_json);
int db_list_users(app_db_t *adb, char **out_json);
