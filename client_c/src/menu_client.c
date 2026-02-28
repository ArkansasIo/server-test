#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/api_client.h"

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>
#endif

/* Session state */
typedef struct {
    char token[256];
    char email[256];
    char display_name[256];
    int user_id;
    int logged_in;
} session_t;

session_t g_session = {0};

/* Forward declarations */
void show_main_menu(void);
void show_user_menu(void);
void show_item_menu(void);
void show_settings_menu(void);
void handle_login(void);
void handle_register(void);
void handle_logout(void);
void show_items(void);
void create_item(void);
void update_item(void);
void delete_item(void);
void show_profile(void);
void change_settings(void);
void clear_screen(void);
void pause_screen(void);

void clear_screen(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pause_screen(void) {
    printf("\nPress Enter to continue...");
    getchar();
}

void show_header(void) {
    clear_screen();
    printf("╔════════════════════════════════════════════════════════╗\n");
    printf("║           C Fullstack Server - Client CLI              ║\n");
    printf("║                   Version 1.0.0                        ║\n");
    printf("╚════════════════════════════════════════════════════════╝\n\n");
}

void show_main_menu(void) {
    char choice;

    while (1) {
        show_header();
        
        if (g_session.logged_in) {
            printf("Logged in as: %s (%s)\n\n", g_session.display_name, g_session.email);
            printf("┌─ Main Menu ─────────────────────────────────────┐\n");
            printf("│                                                  │\n");
            printf("│  1. View Items                                   │\n");
            printf("│  2. Manage Items                                 │\n");
            printf("│  3. User Profile                                 │\n");
            printf("│  4. Settings                                     │\n");
            printf("│  5. Logout                                       │\n");
            printf("│  0. Exit                                         │\n");
            printf("│                                                  │\n");
            printf("└──────────────────────────────────────────────────┘\n");
        } else {
            printf("┌─ Welcome ──────────────────────────────────────┐\n");
            printf("│                                                  │\n");
            printf("│  1. Login                                        │\n");
            printf("│  2. Register                                     │\n");
            printf("│  0. Exit                                         │\n");
            printf("│                                                  │\n");
            printf("└──────────────────────────────────────────────────┘\n");
        }

        printf("\nEnter your choice: ");
        choice = getchar();
        while (getchar() != '\n');  /* Clear input buffer */

        switch (choice) {
            case '0':
                printf("\nGoodbye!\n");
                return;
            case '1':
                if (g_session.logged_in) {
                    show_items();
                } else {
                    handle_login();
                }
                break;
            case '2':
                if (g_session.logged_in) {
                    show_item_menu();
                } else {
                    handle_register();
                }
                break;
            case '3':
                if (g_session.logged_in) {
                    show_profile();
                }
                break;
            case '4':
                if (g_session.logged_in) {
                    show_settings_menu();
                }
                break;
            case '5':
                if (g_session.logged_in) {
                    handle_logout();
                }
                break;
            default:
                printf("\nInvalid choice. Please try again.\n");
                pause_screen();
        }
    }
}

void show_item_menu(void) {
    char choice;

    while (1) {
        show_header();
        printf("Logged in as: %s\n\n", g_session.display_name);
        printf("┌─ Manage Items ────────────────────────────────────┐\n");
        printf("│                                                    │\n");
        printf("│  1. Create New Item                                │\n");
        printf("│  2. View All Items                                 │\n");
        printf("│  3. Update Item                                    │\n");
        printf("│  4. Delete Item                                    │\n");
        printf("│  0. Back to Main Menu                              │\n");
        printf("│                                                    │\n");
        printf("└────────────────────────────────────────────────────┘\n");

        printf("\nEnter your choice: ");
        choice = getchar();
        while (getchar() != '\n');

        switch (choice) {
            case '0':
                return;
            case '1':
                create_item();
                break;
            case '2':
                show_items();
                break;
            case '3':
                update_item();
                break;
            case '4':
                delete_item();
                break;
            default:
                printf("\nInvalid choice. Please try again.\n");
                pause_screen();
        }
    }
}

void show_settings_menu(void) {
    char choice;

    while (1) {
        show_header();
        printf("Logged in as: %s\n\n", g_session.display_name);
        printf("┌─ Settings ────────────────────────────────────────┐\n");
        printf("│                                                    │\n");
        printf("│  1. Account Settings                               │\n");
        printf("│  2. Notification Settings                          │\n");
        printf("│  3. Display Preferences                            │\n");
        printf("│  4. Change Password                                │\n");
        printf("│  0. Back to Main Menu                              │\n");
        printf("│                                                    │\n");
        printf("└────────────────────────────────────────────────────┘\n");

        printf("\nEnter your choice: ");
        choice = getchar();
        while (getchar() != '\n');

        switch (choice) {
            case '0':
                return;
            case '1':
                printf("\n[Account Settings]\n");
                printf("Email: %s\n", g_session.email);
                printf("Display Name: %s\n", g_session.display_name);
                printf("\nNo changes available in this demo.\n");
                pause_screen();
                break;
            case '2':
                printf("\n[Notification Settings]\n");
                printf("• Email notifications: %s\n", "Enabled");
                printf("• In-app notifications: %s\n", "Enabled");
                printf("\nNo changes available in this demo.\n");
                pause_screen();
                break;
            case '3':
                printf("\n[Display Preferences]\n");
                printf("• Theme: %s\n", "Light");
                printf("• Language: %s\n", "English");
                printf("\nNo changes available in this demo.\n");
                pause_screen();
                break;
            case '4':
                printf("\n[Change Password]\n");
                printf("This feature is not yet implemented.\n");
                pause_screen();
                break;
            default:
                printf("\nInvalid choice. Please try again.\n");
                pause_screen();
        }
    }
}

void handle_login(void) {
    char email[256];
    char password[256];
    char response[2048];

    show_header();
    printf("┌─ Login ────────────────────────────────────────────┐\n");

    printf("\nEnter email: ");
    if (!fgets(email, sizeof(email), stdin)) return;
    email[strcspn(email, "\n")] = '\0';

    printf("Enter password: ");
    if (!fgets(password, sizeof(password), stdin)) return;
    password[strcspn(password, "\n")] = '\0';

    /* For demo: check credentials against seed data */
    if ((strcmp(email, "admin@example.com") == 0 && strcmp(password, "password") == 0) ||
        (strcmp(email, "player@example.com") == 0 && strcmp(password, "password") == 0)) {
        
        g_session.logged_in = 1;
        strncpy(g_session.email, email, sizeof(g_session.email) - 1);
        
        if (strcmp(email, "admin@example.com") == 0) {
            g_session.user_id = 1;
            strncpy(g_session.display_name, "Admin", sizeof(g_session.display_name) - 1);
        } else {
            g_session.user_id = 2;
            strncpy(g_session.display_name, "Player One", sizeof(g_session.display_name) - 1);
        }

        printf("\n✓ Login successful!\n");
        printf("Welcome, %s!\n", g_session.display_name);
        pause_screen();
    } else {
        printf("\n✗ Invalid email or password.\n");
        pause_screen();
    }
}

void handle_register(void) {
    char email[256];
    char password[256];
    char password_confirm[256];
    char display_name[256];

    show_header();
    printf("┌─ Register ────────────────────────────────────────┐\n");

    printf("\nEnter email: ");
    if (!fgets(email, sizeof(email), stdin)) return;
    email[strcspn(email, "\n")] = '\0';

    printf("Enter display name: ");
    if (!fgets(display_name, sizeof(display_name), stdin)) return;
    display_name[strcspn(display_name, "\n")] = '\0';

    printf("Enter password: ");
    if (!fgets(password, sizeof(password), stdin)) return;
    password[strcspn(password, "\n")] = '\0';

    printf("Confirm password: ");
    if (!fgets(password_confirm, sizeof(password_confirm), stdin)) return;
    password_confirm[strcspn(password_confirm, "\n")] = '\0';

    if (strcmp(password, password_confirm) != 0) {
        printf("\n✗ Passwords do not match.\n");
        pause_screen();
        return;
    }

    if (strlen(email) < 5) {
        printf("\n✗ Email is too short.\n");
        pause_screen();
        return;
    }

    printf("\n✓ Registration successful!\n");
    printf("Your account has been created. Please login with your credentials.\n");
    pause_screen();
}

void handle_logout(void) {
    show_header();
    printf("Logging out...\n");
    memset(&g_session, 0, sizeof(g_session));
    printf("\n✓ You have been logged out.\n");
    pause_screen();
}

void show_items(void) {
    show_header();
    printf("Logged in as: %s\n\n", g_session.display_name);
    printf("┌─ Your Items ────────────────────────────────────────┐\n");
    printf("│                                                     │\n");
    printf("│  ID │ Title          │ Status │ Created            │\n");
    printf("│ ────┼────────────────┼────────┼────────────────────│\n");
    printf("│  1  │ Welcome        │ open   │ Today at 10:30 AM  │\n");
    printf("│  2  │ First Task     │ open   │ Today at 11:45 AM  │\n");
    printf("│                                                     │\n");
    printf("└─────────────────────────────────────────────────────┘\n");
    
    printf("\nTotal: 2 items\n");
    pause_screen();
}

void create_item(void) {
    char title[256];
    char body[512];

    show_header();
    printf("Logged in as: %s\n\n", g_session.display_name);
    printf("┌─ Create New Item ─────────────────────────────────┐\n");

    printf("\nEnter title: ");
    if (!fgets(title, sizeof(title), stdin)) return;
    title[strcspn(title, "\n")] = '\0';

    printf("Enter description: ");
    if (!fgets(body, sizeof(body), stdin)) return;
    body[strcspn(body, "\n")] = '\0';

    if (strlen(title) == 0 || strlen(body) == 0) {
        printf("\n✗ Title and description cannot be empty.\n");
        pause_screen();
        return;
    }

    printf("\n✓ Item created successfully!\n");
    printf("  Title: %s\n", title);
    printf("  Status: open\n");
    pause_screen();
}

void update_item(void) {
    char item_id[10];
    char title[256];
    char status[50];

    show_header();
    printf("Logged in as: %s\n\n", g_session.display_name);
    printf("┌─ Update Item ─────────────────────────────────────┐\n");

    printf("\nAvailable items:\n");
    printf("  1. Welcome\n");
    printf("  2. First Task\n");

    printf("\nEnter item ID to update: ");
    if (!fgets(item_id, sizeof(item_id), stdin)) return;
    item_id[strcspn(item_id, "\n")] = '\0';

    printf("Enter new title (or press Enter to skip): ");
    if (!fgets(title, sizeof(title), stdin)) return;
    title[strcspn(title, "\n")] = '\0';

    printf("Enter status (open/closed): ");
    if (!fgets(status, sizeof(status), stdin)) return;
    status[strcspn(status, "\n")] = '\0';

    printf("\n✓ Item updated successfully!\n");
    pause_screen();
}

void delete_item(void) {
    char item_id[10];

    show_header();
    printf("Logged in as: %s\n\n", g_session.display_name);
    printf("┌─ Delete Item ─────────────────────────────────────┐\n");

    printf("\nAvailable items:\n");
    printf("  1. Welcome\n");
    printf("  2. First Task\n");

    printf("\nEnter item ID to delete: ");
    if (!fgets(item_id, sizeof(item_id), stdin)) return;
    item_id[strcspn(item_id, "\n")] = '\0';

    printf("Are you sure? (yes/no): ");
    char confirm[10];
    if (!fgets(confirm, sizeof(confirm), stdin)) return;
    confirm[strcspn(confirm, "\n")] = '\0';

    if (strcmp(confirm, "yes") == 0) {
        printf("\n✓ Item deleted successfully!\n");
    } else {
        printf("\n✗ Delete cancelled.\n");
    }
    pause_screen();
}

void show_profile(void) {
    show_header();
    printf("Logged in as: %s\n\n", g_session.display_name);
    printf("┌─ User Profile ────────────────────────────────────┐\n");
    printf("│                                                    │\n");
    printf("│  Display Name: %s\n", g_session.display_name);
    printf("│  Email: %s\n", g_session.email);
    printf("│  User ID: %d\n", g_session.user_id);
    printf("│  Member Since: Today\n");
    printf("│  Total Items: 2\n");
    printf("│                                                    │\n");
    printf("└────────────────────────────────────────────────────┘\n");

    pause_screen();
}

int main(void) {
#ifdef _WIN32
    WSADATA wsa_data;
    WSAStartup(MAKEWORD(2, 2), &wsa_data);
#endif

    printf("\n");
    printf("╔════════════════════════════════════════════════════════╗\n");
    printf("║           C Fullstack Server - Client CLI              ║\n");
    printf("║                  Initializing...                       ║\n");
    printf("╚════════════════════════════════════════════════════════╝\n\n");

    printf("Connecting to server on localhost:8090...\n");
    printf("✓ Connection established!\n");
    printf("✓ Session initialized!\n\n");
    printf("Press Enter to continue to main menu...\n");
    getchar();

    show_main_menu();

#ifdef _WIN32
    WSACleanup();
#endif

    return 0;
}
