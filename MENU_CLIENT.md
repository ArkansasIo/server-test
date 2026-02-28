# Menu Client - Interactive CLI UI

The C Fullstack project now includes an interactive menu-driven CLI client (`menu_client.exe`) that provides a full user interface for accessing the server with authentication, user management, and item management features.

## Features

### Authentication System
- **Login** - Authenticate with email and password
- **Register** - Create a new user account  
- **Logout** - End your session
- **Session Management** - Maintains user context throughout the session

### Main Menu (Unauthenticated)
```
Welcome Page:
  1. Login
  2. Register
  0. Exit
```

### Main Menu (Authenticated)
```
Main Menu:
  1. View Items - Browse all items in the database
  2. Manage Items - Create, update, or delete items
  3. User Profile - View your account information
  4. Settings - Access account settings
  5. Logout - End your session
  0. Exit - Quit the application
```

### Item Management
- **Create New Item** - Add a new item with title and description
- **View All Items** - List all items in table format
- **Update Item** - Modify an existing item's title or status
- **Delete Item** - Remove an item from the database

### User Profile
- View your email and display name
- See account creation date
- View total item count

### Settings Menu
- **Account Settings** - View/manage email and display name
- **Notification Settings** - Configure email and in-app notifications
- **Display Preferences** - Theme and language options
- **Change Password** - Update your password (placeholder for future implementation)

## Running the Menu Client

### Start the Server
```bash
cd fullstack_c_package
cmake --build build
./build/c_fullstack_server.exe --db-config ./server/db.conf --port 8090
```

### Run the Menu Client
In another terminal:
```bash
cd fullstack_c_package
./build/menu_client.exe
```

## Demo Credentials

**Admin Account:**
- Email: `admin@example.com`
- Password: `password`

**User Account:**
- Email: `player@example.com`
- Password: `password`

## Implementation Details

### Architecture
The menu client is a single-threaded, blocking I/O application written in portable C. It provides:

1. **Text-based UI** - Uses Unicode box-drawing characters for visual structure
2. **Menu Navigation** - Hierarchical menu system with back navigation
3. **Session State** - Maintains logged-in user context
4. **Input Handling** - Buffered input with validation
5. **Error Handling** - User-friendly error messages

### Cross-Platform Support
- **Windows**: Winsock2 network support via Winsock initialization
- **Linux/macOS**: POSIX sockets via conditional compilation

### Files
- `client_c/src/menu_client.c` - Main menu client implementation
- `server/src/main.c` - Added auth endpoints (`/api/auth/login`, `/api/auth/register`)
- `server/src/db.h/db.c` - Auth database functions
- `db/schema.sql` - Updated with users and sessions tables

## API Endpoints Added

### Authentication
```
POST /api/auth/login
  Request: { "email": "...", "password": "..." }
  Response: { "ok": true, "user_id": 1, "email": "...", "display_name": "...", "token": "..." }

POST /api/auth/register
  Request: { "email": "...", "password": "...", "display_name": "..." }
  Response: { "ok": true, "user_id": 1, "message": "Registration successful" }

GET /api/users
  Response: { "users": [ { "id": 1, "email": "...", ... }, ... ] }

GET /api/users/:id
  Response: { "ok": true, "id": 1, "email": "...", ... }
```

## Database Schema Updates

### Users Table
```sql
CREATE TABLE users (
  id            INTEGER PRIMARY KEY AUTOINCREMENT,
  email         TEXT NOT NULL UNIQUE,
  password_hash TEXT NOT NULL,
  display_name  TEXT NOT NULL,
  role          TEXT NOT NULL DEFAULT 'user',
  created_at    TEXT NOT NULL DEFAULT (strftime('%Y-%m-%dT%H:%M:%fZ','now')),
  last_login    TEXT
);
```

### Sessions Table
```sql
CREATE TABLE sessions (
  id            INTEGER PRIMARY KEY AUTOINCREMENT,
  user_id       INTEGER NOT NULL,
  token         TEXT NOT NULL UNIQUE,
  expires_at    TEXT NOT NULL,
  created_at    TEXT NOT NULL DEFAULT (strftime('%Y-%m-%dT%H:%M:%fZ','now')),
  FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE
);
```

## Future Enhancements

- [ ] Token-based authentication with JWT
- [ ] Password hashing with bcrypt instead of MD5
- [ ] User role-based access control
- [ ] Form validation and input sanitization
- [ ] Connection timeout handling
- [ ] Pagination for large item lists
- [ ] Search/filter items
- [ ] Batch operations
- [ ] Color support in terminal
- [ ] Help system with context-sensitive tips

## Building from Source

The menu client is built automatically with CMake as part of the project build process:

```bash
cd fullstack_c_package
mkdir build
cd build
cmake ..
cmake --build .
```

The executable will be at `build/menu_client.exe` (Windows) or `build/menu_client` (Unix).

## Notes

- The current implementation uses MD5 hashing for demonstration. In production, use bcrypt or Argon2.
- Password are transmitted in plain JSON (should use HTTPS in production)
- Token handling is simplified for demo purposes
- Real-world usage would require proper input sanitization and SQL injection prevention
