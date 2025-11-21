#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "movie.h"
#include "theatre.h"
#include "show.h"
#include "booking.h"
#include "user.h"
#include "storage.h"

static User *current_user = NULL;

static void seed_demo_data(void) {
    if (movie_count()==0) {
        Movie *m1 = movie_create("The Example: Dawn", 125, 2024);
        Movie *m2 = movie_create("C Adventures", 95, 2023);
        (void)m1; (void)m2;
    }
    if (theatre_count()==0) {
        Theatre *t1 = theatre_create("Central Cinema", "Kolkata");
        Theatre *t2 = theatre_create("City Screens", "Delhi");
        (void)t1; (void)t2;
    }
    if (show_count()==0) {
        // use first movie & theatre
        if (movie_count()>0 && theatre_count()>0) {
            Show *s1 = show_create(1, 1, "2025-11-25 18:00", 8, 10);
            Show *s2 = show_create(2, 2, "2025-11-25 20:00", 6, 8);
            (void)s1; (void)s2;
        }
    }
}

static void prompt_register(void) {
    char username[128], password[128];
    printf("Choose username: ");
    if (scanf("%127s", username)!=1) return;
    printf("Choose password: ");
    if (scanf("%127s", password)!=1) return;
    User *u = user_register(username, password);
    if (!u) puts("Registration failed (maybe username taken).");
    else printf("Registered user id %u\n", u->id);
}

static void prompt_login(void) {
    char username[128], password[128];
    printf("username: ");
    if (scanf("%127s", username)!=1) return;
    printf("password: ");
    if (scanf("%127s", password)!=1) return;
    User *u = user_login(username, password);
    if (!u) puts("Login failed.");
    else { current_user = u; printf("Welcome %s (id %u)\n", u->username, u->id); }
}

static void show_main_menu(void) {
    puts("\n--- Main Menu ---");
    puts("1) Register");
    puts("2) Login");
    puts("3) List movies");
    puts("4) List theatres");
    puts("5) List shows");
    puts("6) Book a seat (must be logged in)");
    puts("7) My bookings");
    puts("8) Save & exit");
    puts("9) Exit without save");
    printf("Choice: ");
}

static void prompt_book_seat(void) {
    if (!current_user) { puts("Please login first."); return; }
    unsigned int sid;
    show_list();
    printf("Enter show id to book: S");
    if (scanf("%u", &sid)!=1) return;
    Show *s = show_find_by_id(sid);
    if (!s) { puts("Invalid show id."); return; }
    // print seat map
    printf("Seat map (0 = free, 1 = booked). Rows: %d, Cols: %d\n", s->rows, s->cols);
    for (int r=0;r<s->rows;++r) {
        for (int c=0;c<s->cols;++c) {
            putchar(s->seats[r*s->cols + c] ? 'X' : 'O');
        }
        putchar('\n');
    }
    int r,c;
    printf("Enter row (0-%d): ", s->rows-1);
    if (scanf("%d", &r)!=1) return;
    printf("Enter col (0-%d): ", s->cols-1);
    if (scanf("%d", &c)!=1) return;
    if (!show_is_seat_free(s, r, c)) { puts("Seat already booked or invalid."); return; }
    Booking *b = booking_create(s->id, current_user->id, r, c);
    if (!b) puts("Booking failed.");
    else printf("Booking success: B%u (Show S%u, Seat %d,%d)\n", b->id, s->id, r, c);
}

static void show_my_bookings(void) {
    if (!current_user) { puts("Please login first."); return; }
    int found = 0;
    puts("Your bookings:");
    // linear scan booking store (simple)
    for (int i=0;i<MAX_BOOKINGS;++i) {
        // We don't have direct access to booking array here; show booking_list_for_show could be used.
        // For demonstration, read bookings file (quick workaround)
    }
    // Instead, print from bookings file
    FILE *f = fopen("bookings.txt", "r");
    if (!f) { puts("No bookings file."); return; }
    unsigned int bid, sid, uid; int rr, cc;
    while (fscanf(f, "%u\t%u\t%u\t%d\t%d\n", &bid, &sid, &uid, &rr, &cc) == 5) {
        if (uid == current_user->id) {
            printf("[B%u] Show S%u Seat %d,%d\n", bid, sid, rr, cc);
            found = 1;
        }
    }
    fclose(f);
    if (!found) puts("  none");
}

int main(void) {
    load_all_data();
    seed_demo_data();

    int choice = 0;
    while (1) {
        show_main_menu();
        if (scanf("%d", &choice)!=1) break;
        switch (choice) {
            case 1: prompt_register(); break;
            case 2: prompt_login(); break;
            case 3: movie_list(); break;
            case 4: theatre_list(); break;
            case 5: show_list(); break;
            case 6: prompt_book_seat(); break;
            case 7: show_my_bookings(); break;
            case 8: save_all_data(); puts("Saved. Goodbye."); return 0;
            case 9: puts("Goodbye."); return 0;
            default: puts("Invalid choice."); break;
        }
    }

    // cleanup
    save_all_data();
    show_free_store();
    booking_free_store();
    return 0;
}
