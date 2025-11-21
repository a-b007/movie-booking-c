#ifndef BOOKING_H
#define BOOKING_H

#include "types.h"

//Booking struct that stores the details of booked seat, show and user.
typedef struct {
    id_t id;
    id_t show_id;
    id_t user_id;
    int row;
    int col;
} Booking;

void booking_init_store(void);
Booking* booking_create(id_t show_id, id_t user_id, int row, int col);
void booking_list_for_show(id_t show_id);
int booking_count(void);
void booking_save_all(void);
void booking_load_all(void);
void booking_free_store(void);

#endif // BOOKING_H
