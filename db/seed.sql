PRAGMA foreign_keys = ON;

INSERT OR IGNORE INTO users (id, email, password_hash, display_name, role) VALUES
  (1, 'admin@example.com', '5e884898da28047151d0e56f8dc62927', 'Admin', 'admin'),
  (2, 'player@example.com', '5e884898da28047151d0e56f8dc62927', 'Player One', 'user');

INSERT OR IGNORE INTO items (id, user_id, title, body, status) VALUES
  (1, 1, 'Welcome', 'Your C full-stack template is running.', 'open'),
  (2, 2, 'First Task', 'Replace this with your own domain objects.', 'open');
