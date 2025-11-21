#include "show.h"
#include "movie.h"
#include "theatre.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Show shows[MAX_SHOWS];
static int shows_used = 0;
static id_t next_show_id = 1;
static const char *SHOW_FILE = "shows.txt";

void show_init_store(void) {
    shows_used = 0;
}

int show_count(void) {
    return shows_used;
}


static unsigned char* alloc_seats(int rows, int cols) {
    if (rows <=0 || cols <=0) return NULL;
    unsigned char *b = calloc(rows * cols, sizeof(unsigned char));
    return b;
}

Show* show_create(id_t movie_id, id_t theatre_id, const char *time_str, int rows, int cols) {
    if (shows_used >= MAX_SHOWS) return NULL;
    if (!movie_find_by_id(movie_id) || !theatre_find_by_id(theatre_id)) return NULL;
    Show *s = &shows[shows_used++];
    s->id = next_show_id++;
    s->movie_id = movie_id;
    s->theatre_id = theatre_id;
    strncpy(s->time_str, time_str, sizeof(s->time_str)-1);
    s->time_str[sizeof(s->time_str)-1]=0;
    s->rows = rows;
    s->cols = cols;
    s->seats = alloc_seats(rows, cols);
    return s;
}

Show* show_find_by_id(id_t id) {
    for (int i=0;i<shows_used;++i) if (shows[i].id == id) return &shows[i];
    return NULL;
}

void show_list(void) {
    if (shows_used==0) { puts("No shows."); return; }
    for (int i=0;i<shows_used;++i) {
        Show *s = &shows[i];
        Movie *m = movie_find_by_id(s->movie_id);
        Theatre *t = theatre_find_by_id(s->theatre_id);
        printf("[S%u] Movie: %s | Theatre: %s | Time: %s | Seats: %dx%d\n",
               s->id,
               m?m->title:"<unknown>",
               t?t->name:"<unknown>",
               s->time_str,
               s->rows, s->cols);
    }
}

int show_is_seat_free(Show *s, int r, int c) {
    if (!s) return 0;
    if (r<0||r>=s->rows||c<0||c>=s->cols) return 0;
    return s->seats[r*s->cols + c] == 0;
}

int show_book_seat(Show *s, int r, int c) {
    if (!s) return 0;
    if (!show_is_seat_free(s, r, c)) return 0;
    s->seats[r*s->cols + c] = 1;
    return 1;
}

void show_free_store(void) {
    for (int i=0;i<shows_used;++i) {
        free(shows[i].seats);
        shows[i].seats = NULL;
    }
    shows_used = 0;
}

void show_save_all(void) {
    FILE *f = fopen(SHOW_FILE, "w");
    if (!f) return;
    for (int i=0;i<shows_used;++i) {
        Show *s = &shows[i];
        fprintf(f, "%u\t%u\t%u\t%s\t%d\t%d\n",
                s->id, s->movie_id, s->theatre_id, s->time_str, s->rows, s->cols);
        // write seat map in rows*cols bytes as 0/1 characters to a second file line
        for (int r=0;r<s->rows;++r) {
            for (int c=0;c<s->cols;++c) {
                fprintf(f, "%d", s->seats[r*s->cols + c]);
            }
            fprintf(f, "\n");
        }
    }
    fclose(f);
}

void show_load_all(void) {
    FILE *f = fopen(SHOW_FILE, "r");
    if (!f) return;
    show_init_store();
    unsigned int id, mid, tid;
    char timestr[64];
    int rows, cols;
    while (fscanf(f, "%u\t%u\t%u\t%63[^\t]\t%d\t%d\n", &id, &mid, &tid, timestr, &rows, &cols) == 6) {
        if (shows_used >= MAX_SHOWS) break;
        Show *s = &shows[shows_used++];
        s->id = id;
        s->movie_id = mid;
        s->theatre_id = tid;
        strncpy(s->time_str, timestr, sizeof(s->time_str)-1); s->time_str[sizeof(s->time_str)-1]=0;
        s->rows = rows; s->cols = cols;
        s->seats = alloc_seats(rows, cols);
        // read next rows lines of seat map
        for (int r=0;r<rows;++r) {
            int ch;
            for (int c=0;c<cols;++c) {
                ch = fgetc(f);
                if (ch == EOF) ch = '0';
                s->seats[r*cols + c] = (ch == '1') ? 1 : 0;
            }
            // consume newline
            while ((ch = fgetc(f)) != '\n' && ch != EOF) { if (ch=='\r') break; }
        }
        if (id >= next_show_id) next_show_id = id + 1;
    }
    fclose(f);
}
