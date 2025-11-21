#include "booking.h"
#include "show.h"
#include "user.h"
#include <stdio.h>
#include <stdlib.h>

//Static data storage

static Booking bookings[MAX_BOOKINGS];                         //static struct that stores booking details
static int bookings_used = 0;                                  //number of bookings done
static id_t next_booking_id = 1;                               //next booking
static const char *BOOKING_FILE = "bookings.txt";              //

void booking_init_store(void) {
    bookings_used = 0;
}

Booking* booking_create(id_t show_id, id_t user_id, int row, int col) {
    if (bookings_used >= MAX_BOOKINGS) return NULL;
    Show *s = show_find_by_id(show_id);
    if (!s) return NULL;
    if (!user_find_by_id(user_id)) return NULL;
    if (!show_is_seat_free(s, row, col)) return NULL;
    if (!show_book_seat(s, row, col)) return NULL;
    Booking *b = &bookings[bookings_used++];
    b->id = next_booking_id++;
    b->show_id = show_id;
    b->user_id = user_id;
    b->row = row;
    b->col = col;
    return b;
}

void booking_list_for_show(id_t show_id) {
    puts("Bookings for show:");
    int found = 0;
    for (int i=0;i<bookings_used;++i) {
        Booking *b = &bookings[i];
        if (b->show_id == show_id) {
            printf("[B%u] User %u Seat: %d,%d\n", b->id, b->user_id, b->row, b->col);
            found = 1;
        }
    }
    if (!found) puts("  none");
}

int booking_count(void) { return bookings_used; }

void booking_save_all(void) {
    FILE *f = fopen(BOOKING_FILE, "w");
    if (!f) return;
    for (int i=0;i<bookings_used;++i) {
        Booking *b = &bookings[i];
        fprintf(f, "%u\t%u\t%u\t%d\t%d\n", b->id, b->show_id, b->user_id, b->row, b->col);
    }
    fclose(f);
}

void booking_load_all(void) {
    FILE *f = fopen(BOOKING_FILE, "r");
    if (!f) return;
    booking_init_store();
    unsigned int id, sid, uid;
    int r, c;
    while (fscanf(f, "%u\t%u\t%u\t%d\t%d\n", &id, &sid, &uid, &r, &c) == 5) {
        if (bookings_used >= MAX_BOOKINGS) break;
        Booking *b = &bookings[bookings_used++];
        b->id = id; b->show_id = sid; b->user_id = uid; b->row = r; b->col = c;
        // Mark seat as booked in show if possible
        Show *s = show_find_by_id(sid);
        if (s && show_is_seat_free(s, r, c)) {
            show_book_seat(s, r, c);
        }
        if (id >= next_booking_id) next_booking_id = id + 1;
    }
    fclose(f);
}

void booking_free_store(void) {
    bookings_used = 0;
}
