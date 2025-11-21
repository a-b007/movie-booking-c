#ifndef SHOW_H
#define SHOW_H

#include "types.h"

typedef struct {
    id_t id;
    id_t movie_id;
    id_t theatre_id;
    char time_str[64]; // simple timestamp string/label
    int rows;
    int cols;
    // seats: 0 = free, 1 = booked
    unsigned char *seats; // rows * cols bytes
} Show;

void show_init_store(void);
Show* show_create(id_t movie_id, id_t theatre_id, const char *time_str, int rows, int cols);
Show* show_find_by_id(id_t id);
void show_list(void);
int show_count(void);
int show_is_seat_free(Show *s, int r, int c);
int show_book_seat(Show *s, int r, int c);
void show_free_store(void);
void show_save_all(void);
void show_load_all(void);

#endif // SHOW_H
