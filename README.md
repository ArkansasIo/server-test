# C Full-Stack Package (SQLite + REST API + Static Front-End + C Client SDK + Interactive CLI)

This repo is a **single package** that includes:

- **SQL database** (SQLite) with `schema.sql` + `seed.sql`
- **Backend API server** written in **C** (REST-ish JSON endpoints)
- **Front-end app** (HTML/JS) served by the same C server binary
- **Front-end client API package**: a small **C client library** (`client_c`) + example
- **Interactive CLI menu client** - Complete user interface with authentication, item management, and settings

## Quick Start - Menu Client

Experience the full C fullstack with our interactive CLI interface:

### Windows
```bash
cd fullstack_c_package
./run-menu-client.bat
```

### Linux/macOS
```bash
cd fullstack_c_package
chmod +x run-menu-client.sh
./run-menu-client.sh
```

**Demo Credentials:**
- Email: `admin@example.com` | Password: `password`
- Email: `player@example.com` | Password: `password`

For full menu client documentation, see [MENU_CLIENT.md](MENU_CLIENT.md), [UI_FEATURES.md](UI_FEATURES.md), and [UI_MOCKUP.md](UI_MOCKUP.md).

## Requirements

- CMake >= 3.16
- A C compiler (clang or gcc)
- SQLite3 dev library (optional; bundled SQLite is used by default)

## Quick start (Docker)

```bash
docker-compose up
# then open http://localhost:8080
```

## Build

```bash
make build
```

If you want to use system SQLite instead of bundled SQLite, configure with:

```bash
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_C_COMPILER=clang -DUSE_BUNDLED_SQLITE=OFF -DSQLITE3_INCLUDE_DIR=/path/to/include -DSQLITE3_LIBRARY=/path/to/libsqlite3.a
cmake --build build
```

## Run the server

```bash
make run
# then open:
#   http://localhost:8080/
#   http://localhost:8080/api/health
#   http://localhost:8080/api/items
```

Server flags:

```bash
./build/c_fullstack_server --db-config ./server/db.conf
./build/c_fullstack_server --port 8080 --db ./data/app.db --public ./server/public
```

## Database config and options

The server loads DB/runtime config from `server/db.conf` (Key=Value format).

Precedence (highest to lowest):
- CLI flags
- Environment variables
- `server/db.conf`
- Built-in defaults

Supported config keys in `server/db.conf`:
- `PORT`
- `PUBLIC_DIR`
- `DB_PATH`
- `DB_SCHEMA`
- `DB_SEED_FILE`
- `DB_ENABLE_SEED` (`0|1`, `true|false`, `on|off`, `yes|no`)
- `DB_JOURNAL_MODE` (`WAL`, `DELETE`, `TRUNCATE`, `MEMORY`, `OFF`)
- `DB_FOREIGN_KEYS` (`0|1`)
- `DB_BUSY_TIMEOUT_MS` (integer)

Equivalent environment variables:
- `APP_PORT`
- `APP_PUBLIC_DIR`
- `APP_DB_PATH`
- `APP_DB_SCHEMA`
- `APP_DB_SEED_FILE`
- `APP_DB_ENABLE_SEED`
- `APP_DB_JOURNAL_MODE`
- `APP_DB_FOREIGN_KEYS`
- `APP_DB_BUSY_TIMEOUT_MS`

Related CLI flags:
- `--port <n>`
- `--public <dir>`
- `--db <path>`
- `--db-schema <path>`
- `--db-seed <path>`
- `--db-seed-enable <0|1>`
- `--db-journal <mode>`
- `--db-foreign-keys <0|1>`
- `--db-timeout-ms <n>`
- `--db-config <path>`

## API

**Authentication**
- `POST /api/auth/login` - Authenticate with email/password
- `POST /api/auth/register` - Create new user account
- `GET /api/users` - List all users
- `GET /api/users/:id` - Get user details

**Items**
- `GET /api/health` -> JSON
- `GET /api/items` -> JSON list (max 100)
- `POST /api/items` body:
  ```json
  { "user_id": 1, "title": "abc", "body": "xyz" }
  ```

## Client SDK (C)

Build example:

```bash
cmake -S . -B build
cmake --build build -j
./build/client_example
```

## Excel schema

See `excel/schema.xlsx` for a human-friendly view of tables/columns/keys and sample rows.

> Notes:
> - The HTTP server is intentionally minimal (single-threaded, simple parsing) for a clean starting point.
> - For production, swap the HTTP layer with a mature embedded server (civetweb, mongoose, etc.) and add auth, validation, pagination, etc.
