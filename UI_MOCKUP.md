# Menu Client - Visual UI Mockup

This document shows what the menu client looks like when running.

## Startup Screen

```
╔════════════════════════════════════════════════════════╗
║           C Fullstack Server - Client CLI              ║
║                   Version 1.0.0                        ║
╚════════════════════════════════════════════════════════╝

Connecting to server on localhost:8090...
✓ Connection established!
✓ Session initialized!

Press Enter to continue to main menu...
```

## Welcome Screen (Unauthenticated)

```
╔════════════════════════════════════════════════════════╗
║           C Fullstack Server - Client CLI              ║
║                   Version 1.0.0                        ║
╚════════════════════════════════════════════════════════╝

┌─ Welcome ──────────────────────────────────────────┐
│                                                    │
│  1. Login                                          │
│  2. Register                                       │
│  0. Exit                                           │
│                                                    │
└────────────────────────────────────────────────────┘

Enter your choice: 
```

## Login Form

```
╔════════════════════════════════════════════════════════╗
║           C Fullstack Server - Client CLI              ║
║                   Version 1.0.0                        ║
╚════════════════════════════════════════════════════════╝

┌─ Login ────────────────────────────────────────────┐

Enter email: admin@example.com
Enter password: ________

✓ Login successful!
Welcome, Admin!

Press Enter to continue...
```

## Main Menu (Authenticated)

```
╔════════════════════════════════════════════════════════╗
║           C Fullstack Server - Client CLI              ║
║                   Version 1.0.0                        ║
╚════════════════════════════════════════════════════════╝

Logged in as: Admin (admin@example.com)

┌─ Main Menu ─────────────────────────────────────────┐
│                                                     │
│  1. View Items                                      │
│  2. Manage Items                                    │
│  3. User Profile                                    │
│  4. Settings                                        │
│  5. Logout                                          │
│  0. Exit                                            │
│                                                     │
└─────────────────────────────────────────────────────┘

Enter your choice: 
```

## Items List View

```
╔════════════════════════════════════════════════════════╗
║           C Fullstack Server - Client CLI              ║
║                   Version 1.0.0                        ║
╚════════════════════════════════════════════════════════╝

Logged in as: Admin

┌─ Your Items ────────────────────────────────────────┐
│                                                     │
│  ID │ Title          │ Status │ Created            │
│ ────┼────────────────┼────────┼────────────────────│
│  1  │ Welcome        │ open   │ Today at 10:30 AM  │
│  2  │ First Task     │ open   │ Today at 11:45 AM  │
│                                                     │
└─────────────────────────────────────────────────────┘

Total: 2 items

Press Enter to continue...
```

## Manage Items Submenu

```
╔════════════════════════════════════════════════════════╗
║           C Fullstack Server - Client CLI              ║
║                   Version 1.0.0                        ║
╚════════════════════════════════════════════════════════╝

Logged in as: Admin

┌─ Manage Items ────────────────────────────────────┐
│                                                    │
│  1. Create New Item                                │
│  2. View All Items                                 │
│  3. Update Item                                    │
│  4. Delete Item                                    │
│  0. Back to Main Menu                              │
│                                                    │
└────────────────────────────────────────────────────┘

Enter your choice: 
```

## Create Item Form

```
╔════════════════════════════════════════════════════════╗
║           C Fullstack Server - Client CLI              ║
║                   Version 1.0.0                        ║
╚════════════════════════════════════════════════════════╝

Logged in as: Admin

┌─ Create New Item ─────────────────────────────────┐

Enter title: My New Task
Enter description: Complete this amazing feature

✓ Item created successfully!
  Title: My New Task
  Status: open

Press Enter to continue...
```

## User Profile Screen

```
╔════════════════════════════════════════════════════════╗
║           C Fullstack Server - Client CLI              ║
║                   Version 1.0.0                        ║
╚════════════════════════════════════════════════════════╝

Logged in as: Admin

┌─ User Profile ────────────────────────────────────┐
│                                                    │
│  Display Name: Admin                               │
│  Email: admin@example.com                          │
│  User ID: 1                                        │
│  Member Since: Today                               │
│  Total Items: 3                                    │
│                                                    │
└────────────────────────────────────────────────────┘

Press Enter to continue...
```

## Settings Menu

```
╔════════════════════════════════════════════════════════╗
║           C Fullstack Server - Client CLI              ║
║                   Version 1.0.0                        ║
╚════════════════════════════════════════════════════════╝

Logged in as: Admin

┌─ Settings ────────────────────────────────────────┐
│                                                    │
│  1. Account Settings                               │
│  2. Notification Settings                          │
│  3. Display Preferences                            │
│  4. Change Password                                │
│  0. Back to Main Menu                              │
│                                                    │
└────────────────────────────────────────────────────┘

Enter your choice: 
```

## Account Settings Submenu

```
╔════════════════════════════════════════════════════════╗
║           C Fullstack Server - Client CLI              ║
║                   Version 1.0.0                        ║
╚════════════════════════════════════════════════════════╝

Logged in as: Admin

[Account Settings]
Email: admin@example.com
Display Name: Admin

No changes available in this demo.

Press Enter to continue...
```

## Notification Settings Submenu

```
╔════════════════════════════════════════════════════════╗
║           C Fullstack Server - Client CLI              ║
║                   Version 1.0.0                        ║
╚════════════════════════════════════════════════════════╝

Logged in as: Admin

[Notification Settings]
• Email notifications: Enabled
• In-app notifications: Enabled

No changes available in this demo.

Press Enter to continue...
```

## Display Preferences Submenu

```
╔════════════════════════════════════════════════════════╗
║           C Fullstack Server - Client CLI              ║
║                   Version 1.0.0                        ║
╚════════════════════════════════════════════════════════╝

Logged in as: Admin

[Display Preferences]
• Theme: Light
• Language: English

No changes available in this demo.

Press Enter to continue...
```

## Registration Form

```
╔════════════════════════════════════════════════════════╗
║           C Fullstack Server - Client CLI              ║
║                   Version 1.0.0                        ║
╚════════════════════════════════════════════════════════╝

┌─ Register ────────────────────────────────────────┐

Enter email: newuser@example.com
Enter display name: New User
Enter password: ________
Confirm password: ________

✓ Registration successful!
Your account has been created. Please login with your credentials.

Press Enter to continue...
```

## Delete Item Confirmation

```
╔════════════════════════════════════════════════════════╗
║           C Fullstack Server - Client CLI              ║
║                   Version 1.0.0                        ║
╚════════════════════════════════════════════════════════╝

Logged in as: Admin

┌─ Delete Item ─────────────────────────────────────┐

Available items:
  1. Welcome
  2. First Task

Enter item ID to delete: 1
Are you sure? (yes/no): yes

✓ Item deleted successfully!

Press Enter to continue...
```

## Update Item Form

```
╔════════════════════════════════════════════════════════╗
║           C Fullstack Server - Client CLI              ║
║                   Version 1.0.0                        ║
╚════════════════════════════════════════════════════════╝

Logged in as: Admin

┌─ Update Item ─────────────────────────────────────┐

Available items:
  1. Welcome
  2. First Task

Enter item ID to update: 1
Enter new title (or press Enter to skip): Welcome Updated
Enter status (open/closed): closed

✓ Item updated successfully!

Press Enter to continue...
```

## Logout Confirmation

```
╔════════════════════════════════════════════════════════╗
║           C Fullstack Server - Client CLI              ║
║                   Version 1.0.0                        ║
╚════════════════════════════════════════════════════════╝

Logging out...

✓ You have been logged out.

Press Enter to continue...
```

## User Flow Diagram

```
┌─────────────────────┐
│   Start Program     │
└──────────┬──────────┘
           │
           ▼
┌─────────────────────┐
│  Welcome Screen     │
├─────────────────────┤
│  1. Login           │
│  2. Register        │
│  0. Exit            │
└──────────┬──────────┘
           │
    ┌──────┴──────┐
    │             │
    ▼             ▼
  Login      Register
    │             │
    ▼             ▼
┌─────────────────────┐
│ Authenticated       │
├─────────────────────┤
│ Main Menu           │
│ 1. View Items       │
│ 2. Manage Items     │
│ 3. User Profile     │
│ 4. Settings         │
│ 5. Logout           │
│ 0. Exit             │
└──────────┬──────────┘
           │
     ┌─────┼──────┬────────┬──────────┐
     │     │      │        │          │
     ▼     ▼      ▼        ▼          ▼
   View  Manage Profile Settings   Logout
  Items  Items
     │     │      │        │          │
     │     ▼      │        │          │
     │  ┌─────┐   │        │          │
     │  │ CRUD│   │        │          │
     │  └─────┘   │        │          │
     │      │     │        │          │
     └──────┴─────┴────────┴──────────┘
            │
            ▼
     Back to Main Menu
            │
            └──────┐
                   │
              Exit?└─► ✓ End Program
```

## Input Examples

### Valid Login
```
Email: admin@example.com
Password: password
Result: ✓ Login successful!
```

### Invalid Login
```
Email: admin@example.com
Password: wrongpass
Result: ✗ Invalid email or password.
```

### Valid Registration
```
Email: test@example.com
Display Name: Test User
Password: mypassword
Confirm: mypassword
Result: ✓ Registration successful!
```

### Password Mismatch
```
Password: mypassword
Confirm: different
Result: ✗ Passwords do not match.
```

---

This visual guide shows the complete user interface of the menu client. The actual experience is identical to these mockups.
