#include "movie.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static Movie movies[MAX_MOVIES];
static int movies_used = 0;
static id_t next_movie_id = 1;
static const char *MOVIE_FILE = "movies.txt";

void movie_init_store(void) {
    movies_used = 0;
}

Movie* movie_create(const char *title, int duration_min, int year) {
    if (movies_used >= MAX_MOVIES) return NULL;
    Movie *m = &movies[movies_used++];
    m->id = next_movie_id++;
    strncpy(m->title, title, MAX_NAME-1);
    m->title[MAX_NAME-1] = '\0';
    m->duration_min = duration_min;
    m->year = year;
    return m;
}

Movie* movie_find_by_id(id_t id) {
    for (int i = 0; i < movies_used; ++i) if (movies[i].id == id) return &movies[i];
    return NULL;
}

void movie_list(void) {
    if (movies_used == 0) { puts("No movies."); return; }
    for (int i=0;i<movies_used;++i) {
        Movie *m = &movies[i];
        printf("[M%u] %s (%d min, %d)\n", m->id, m->title, m->duration_min, m->year);
    }
}

int movie_count(void) { return movies_used; }

void movie_save_all(void) {
    FILE *f = fopen(MOVIE_FILE, "w");
    if (!f) return;
    for (int i = 0; i < movies_used; ++i) {
        Movie *m = &movies[i];
        fprintf(f, "%u\t%s\t%d\t%d\n", m->id, m->title, m->duration_min, m->year);
    }
    fclose(f);
}

void movie_load_all(void) {
    FILE *f = fopen(MOVIE_FILE, "r");
    if (!f) return;
    movie_init_store();
    unsigned int id;
    char title[MAX_NAME];
    int dur, yr;
    while (fscanf(f, "%u\t%127[^\t]\t%d\t%d\n", &id, title, &dur, &yr) == 4) {
        Movie *m = &movies[movies_used++];
        m->id = id;
        strncpy(m->title, title, MAX_NAME-1);
        m->title[MAX_NAME-1] = '\0';
        m->duration_min = dur;
        m->year = yr;
        if (id >= next_movie_id) next_movie_id = id + 1;
        if (movies_used >= MAX_MOVIES) break;
    }
    fclose(f);
}
