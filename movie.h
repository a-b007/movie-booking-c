#ifndef MOVIE_H
#define MOVIE_H

#include "types.h"

typedef struct {
    id_t id;
    char title[MAX_NAME];
    int duration_min; // movie duration in minutes
    int year;
} Movie;

// Movie store functions
void movie_init_store(void);
Movie* movie_create(const char *title, int duration_min, int year);
Movie* movie_find_by_id(id_t id);
void movie_list(void);
int movie_count(void);
void movie_save_all(void);
void movie_load_all(void);

#endif // MOVIE_H
