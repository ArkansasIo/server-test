PRAGMA foreign_keys = ON;

INSERT OR IGNORE INTO users (id, email, display_name) VALUES
  (1, 'admin@example.com', 'Admin'),
  (2, 'player@example.com', 'Player One');

INSERT OR IGNORE INTO items (id, user_id, title, body, status) VALUES
  (1, 1, 'Welcome', 'Your C full-stack template is running.', 'open'),
  (2, 2, 'First Task', 'Replace this with your own domain objects.', 'open');
