// convenience: load/save all store modules
#include "movie.h"
#include "theatre.h"
#include "show.h"
#include "booking.h"
#include "user.h"

void save_all_data(void) {
    movie_save_all();
    theatre_save_all();
    show_save_all();
    booking_save_all();
    user_save_all();
}

void load_all_data(void) {
    movie_load_all();
    theatre_load_all();
    show_load_all();
    user_load_all();
    booking_load_all();
}
