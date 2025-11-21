#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/types.h"
#include <sys/stat.h>
#include <sys/types.h>


/* Simple binary storage layer for Movie, Theatre, Show */

static const char *MOVIES_FILE = "data/movies.bin";
static const char *THEATRES_FILE = "data/theatres.bin";
static const char *SHOWS_FILE = "data/shows.bin";

/* ensure data directory exists (best-effort) */
static void ensure_data_dir() {
    /* POSIX mkdir */
#ifdef _WIN32
    _mkdir("data");
#else
    mkdir("data", 0755);
#endif
}

/* Generic append/read for arrays */
int append_movie(const Movie *m) {
    ensure_data_dir();
    FILE *f = fopen(MOVIES_FILE, "ab");
    if (!f) return 0;
    fwrite(m, sizeof(Movie), 1, f);
    fclose(f);
    return 1;
}

int read_all_movies(Movie **out, size_t *count) {
    *out = NULL; *count = 0;
    FILE *f = fopen(MOVIES_FILE, "rb");
    if (!f) return 0;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    if (sz <= 0) { fclose(f); return 0; }
    rewind(f);
    size_t n = sz / sizeof(Movie);
    *out = malloc(n * sizeof(Movie));
    fread(*out, sizeof(Movie), n, f);
    fclose(f);
    *count = n; return 1;
}

int append_theatre(const Theatre *t) {
    ensure_data_dir();
    FILE *f = fopen(THEATRES_FILE, "ab"); if(!f) return 0;
    fwrite(t, sizeof(Theatre), 1, f); fclose(f); return 1;
}
int read_all_theatres(Theatre **out, size_t *count) {
    *out = NULL; *count = 0;
    FILE *f = fopen(THEATRES_FILE, "rb"); if(!f) return 0;
    fseek(f,0,SEEK_END); long sz=ftell(f); if(sz<=0){fclose(f);return 0;} rewind(f);
    size_t n = sz/sizeof(Theatre); *out = malloc(n*sizeof(Theatre)); fread(*out,sizeof(Theatre),n,f); fclose(f); *count=n; return 1;
}

int append_show(const Show *s) {
    ensure_data_dir();
    FILE *f = fopen(SHOWS_FILE, "ab"); if(!f) return 0;
    fwrite(s, sizeof(Show), 1, f); fclose(f); return 1;
}

int read_all_shows(Show **out, size_t *count) {
    *out = NULL; *count = 0;
    FILE *f = fopen(SHOWS_FILE, "rb"); if(!f) return 0;
    fseek(f,0,SEEK_END); long sz=ftell(f); if(sz<=0){fclose(f);return 0;} rewind(f);
    size_t n = sz/sizeof(Show); *out = malloc(n*sizeof(Show)); fread(*out,sizeof(Show),n,f); fclose(f); *count=n; return 1;
}

/* Rewrite entire shows file from memory (used after modifying booked_csv) */
int overwrite_all_shows(Show *shows, size_t count) {
    ensure_data_dir();
    FILE *f = fopen(SHOWS_FILE, "wb"); if(!f) return 0;
    fwrite(shows, sizeof(Show), count, f); fclose(f); return 1;
}

/* Helpers to assign next IDs */
int next_movie_id() {
    Movie *a; size_t n; if (!read_all_movies(&a,&n)) return 1;
    int last = 0; for (size_t i=0;i<n;i++) if (a[i].id > last) last = a[i].id; free(a); return last+1;
}
int next_theatre_id() {
    Theatre *a; size_t n; if (!read_all_theatres(&a,&n)) return 1;
    int last=0; for(size_t i=0;i<n;i++) if(a[i].id>last) last=a[i].id; free(a); return last+1;
}
int next_show_id() {
    Show *a; size_t n; if (!read_all_shows(&a,&n)) return 1;
    int last=0; for(size_t i=0;i<n;i++) if(a[i].id>last) last=a[i].id; free(a); return last+1;
}

/* Expose file paths to other modules (simple) */
const char *storage_movies_file() { return MOVIES_FILE; }
const char *storage_theatres_file() { return THEATRES_FILE; }
const char *storage_shows_file() { return SHOWS_FILE; }

/* Small utility: overwrite shows by reading then writing back with modifications */
int update_show(Show *updated) {
    Show *arr; size_t n; if(!read_all_shows(&arr,&n)) return 0;
    int found = 0; for(size_t i=0;i<n;i++) { if(arr[i].id == updated->id) { arr[i] = *updated; found = 1; break; } }
    if (!found) { free(arr); return 0; }
    int ok = overwrite_all_shows(arr, n);
    free(arr); return ok;
}

/* read a single show by id into dest; returns 1 on success */
int read_show_by_id(int id, Show *dest) {
    Show *arr; size_t n; if(!read_all_shows(&arr,&n)) return 0;
    int ok = 0; for(size_t i=0;i<n;i++) { if (arr[i].id==id) { *dest = arr[i]; ok = 1; break; } }
    free(arr); return ok;
}

/* read movie name */
int read_movie_by_id(int id, Movie *dest) {
    Movie *arr; size_t n; if(!read_all_movies(&arr,&n)) return 0;
    int ok=0; for(size_t i=0;i<n;i++) if(arr[i].id==id){ *dest = arr[i]; ok=1; break; }
    free(arr); return ok;
}
int read_theatre_by_id(int id, Theatre *dest) {
    Theatre *arr; size_t n; if(!read_all_theatres(&arr,&n)) return 0;
    int ok=0; for(size_t i=0;i<n;i++) if(arr[i].id==id){ *dest = arr[i]; ok=1; break; }
    free(arr); return ok;
}
