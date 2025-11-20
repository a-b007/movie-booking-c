/*
Full Booking Management System (C) - Rewritten with structs
Features:
 - Struct-based in-memory model for Movies, Theatres, Shows
 - Binary storage for entities (movies, theatres, shows)
 - CSV-style storage for bookings inside show records
 - Multi-seat booking
 - Seat cancellation
 - Simple ASCII animations for booking/cancellation
 - Cleaner APIs, safer parsing and bounds checking

Files included below in a single document (split by comments):
 - include/types.h
 - include/movie.h
 - include/theatre.h
 - include/show.h
 - include/user.h
 - include/booking.h
 - src/storage.c
 - src/movie.c
 - src/theatre.c
 - src/show.c
 - src/user.c
 - src/booking.c
 - src/main.c
 - Makefile

Build: make
Run: ./app

Note: This code aims to be portable but uses usleep() for animation (POSIX). On Windows, replace usleep with Sleep.
*/

/* ===== include/types.h ===== */
#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#define NAME_LEN 128
#define TIME_LEN 32

typedef struct {
    int id;                 /* 1-based */
    char name[NAME_LEN];
} Movie;

typedef struct {
    int id;
    char name[NAME_LEN];
    char location[NAME_LEN];
} Theatre;

/* Show stores rows x cols seat map and a comma-separated booked-seat CSV in file */
typedef struct {
    int id;
    int movie_id;          /* index into movies file */
    int theatre_id;
    char time[TIME_LEN];
    int rows;
    int cols;
    char booked_csv[4096]; /* holds comma separated seat ids: A1,B2,C3... */
} Show;

#endif

/* ===== include/movie.h ===== */
#ifndef MOVIE_H
#define MOVIE_H

void admin_add_movie();
void list_movies();
int get_movie_count();
int get_movie_id_by_index(int idx);
char *get_movie_name_by_id(int id, char *buf, int buflen);

#endif

/* ===== include/theatre.h ===== */
#ifndef THEATRE_H
#define THEATRE_H

void admin_add_theatre();
void list_theatres();
int get_theatre_count();
int get_theatre_id_by_index(int idx);
char *get_theatre_name_by_id(int id, char *buf, int buflen);

#endif

/* ===== include/show.h ===== */
#ifndef SHOW_H
#define SHOW_H

void admin_add_show();
void list_shows();
int get_show_count();
int get_show_id_by_index(int idx);
void get_show_dimensions(int show_id, int *rows, int *cols);
void get_show_info(int show_id, int *movie_idx, int *theatre_idx, char *timebuf, int timebuflen);
int is_seat_booked(int show_id, const char *seat);
int book_seats_for_show(int show_id, const char *seats_csv); /* seats_csv: "A1,A2" */
int cancel_seats_for_show(int show_id, const char *seats_csv);

#endif

/* ===== include/user.h ===== */
#ifndef USER_H
#define USER_H

void register_user();
void login_user();

#endif

/* ===== include/booking.h ===== */
#ifndef BOOKING_H
#define BOOKING_H

void user_dashboard(const char *username);

#endif

/* ===== src/storage.c ===== */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/types.h"

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

/* end storage.c */

/* ===== src/movie.c ===== */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/types.h"
#include "../include/movie.h"

/* storage helpers declared in storage.c (forward) */
int append_movie(const Movie *m);
int read_all_movies(Movie **out, size_t *count);
int next_movie_id();

void admin_add_movie() {
    char name[NAME_LEN]; while(getchar()!='\n');
    printf("Enter movie name: "); fgets(name, sizeof(name), stdin); name[strcspn(name, "\n")] = 0;
    if (strlen(name)==0) { printf("Name empty, abort.\n"); return; }
    Movie m = {0}; m.id = next_movie_id(); strncpy(m.name, name, NAME_LEN-1);
    if (append_movie(&m)) printf("Added movie %s (id=%d)\n", m.name, m.id);
    else printf("Failed to add movie\n");
}

void list_movies() {
    Movie *arr; size_t n; if(!read_all_movies(&arr,&n)) { printf("No movies available.\n"); return; }
    printf("\nAvailable Movies:\n"); for(size_t i=0;i<n;i++) printf("%zu. %s (id=%d)\n", i+1, arr[i].name, arr[i].id);
    free(arr);
}

int get_movie_count() { Movie *a; size_t n; if(!read_all_movies(&a,&n)) return 0; free(a); return (int)n; }
int get_movie_id_by_index(int idx) { Movie *a; size_t n; if(!read_all_movies(&a,&n)) return -1; if (idx<1 || idx> (int)n) { free(a); return -1; } int id=a[idx-1].id; free(a); return id; }
char *get_movie_name_by_id(int id, char *buf, int buflen) { Movie m; if(!read_movie_by_id(id, &m)) { buf[0]=0; return buf; } strncpy(buf, m.name, buflen-1); buf[buflen-1]=0; return buf; }

/* ===== src/theatre.c ===== */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/types.h"
#include "../include/theatre.h"

int append_theatre(const Theatre *t);
int read_all_theatres(Theatre **out, size_t *count);
int next_theatre_id();
int read_theatre_by_id(int id, Theatre *dest);

void admin_add_theatre() {
    char name[NAME_LEN], loc[NAME_LEN]; while(getchar()!='\n');
    printf("Enter theatre name: "); fgets(name, sizeof(name), stdin); name[strcspn(name, "\n")] = 0;
    printf("Enter location: "); fgets(loc, sizeof(loc), stdin); loc[strcspn(loc, "\n")] = 0;
    if (strlen(name)==0) { printf("Name empty, abort.\n"); return; }
    Theatre t = {0}; t.id = next_theatre_id(); strncpy(t.name, name, NAME_LEN-1); strncpy(t.location, loc, NAME_LEN-1);
    if (append_theatre(&t)) printf("Added theatre %s (%s) id=%d\n", t.name, t.location, t.id);
    else printf("Failed to add theatre\n");
}

void list_theatres() {
    Theatre *arr; size_t n; if(!read_all_theatres(&arr,&n)) { printf("No theatres available.\n"); return; }
    printf("\nAvailable Theatres:\n"); for(size_t i=0;i<n;i++) printf("%zu. %s (%s) id=%d\n", i+1, arr[i].name, arr[i].location, arr[i].id);
    free(arr);
}

int get_theatre_count() { Theatre *a; size_t n; if(!read_all_theatres(&a,&n)) return 0; free(a); return (int)n; }
int get_theatre_id_by_index(int idx) { Theatre *a; size_t n; if(!read_all_theatres(&a,&n)) return -1; if (idx<1 || idx> (int)n) { free(a); return -1; } int id=a[idx-1].id; free(a); return id; }
char *get_theatre_name_by_id(int id, char *buf, int buflen) { Theatre t; if(!read_theatre_by_id(id, &t)) { buf[0]=0; return buf; } strncpy(buf, t.name, buflen-1); buf[buflen-1]=0; return buf; }

/* ===== src/show.c ===== */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h> /* usleep for animation */
#include "../include/types.h"
#include "../include/show.h"

int append_show(const Show *s);
int read_all_shows(Show **out, size_t *count);
int next_show_id();
int update_show(Show *updated);
int read_show_by_id(int id, Show *dest);
int read_movie_by_id(int id, Movie *dest);
int read_theatre_by_id(int id, Theatre *dest);

/* Helper: normalize seat token (remove leading spaces) */
static char *ltrim(char *s) { while (*s==' '||*s=='\t') s++; return s; }

/* check seat validity A1.. */
static int seat_to_rc(const char *seat, int *out_r, int *out_c, int rows, int cols) {
    if (!seat || strlen(seat)<2) return 0;
    char rowch = seat[0]; if (!isalpha((unsigned char)rowch)) return 0;
    int r = toupper(rowch) - 'A'; int c = atoi(seat+1) - 1; if (r<0 || r>=rows || c<0 || c>=cols) return 0;
    *out_r = r; *out_c = c; return 1;
}

/* split CSV into tokens, calling func for each token */
static void for_each_seat_token(const char *csv, void (*fn)(const char *, void *), void *ud) {
    if (!csv) return; char tmp[4096]; strncpy(tmp, csv, sizeof(tmp)-1); tmp[sizeof(tmp)-1]=0;
    char *tok = strtok(tmp, ","); while(tok) { char *s = ltrim(tok); fn(s, ud); tok = strtok(NULL, ","); }
}

static int seat_in_csv(const char *csv, const char *seat) {
    if (!csv || csv[0]==0) return 0; char tmp[4096]; strncpy(tmp,csv,sizeof(tmp)-1); tmp[sizeof(tmp)-1]=0;
    char *tok = strtok(tmp, ","); while(tok) { char *s=ltrim(tok); if (strcmp(s, seat)==0) return 1; tok = strtok(NULL, ","); }
    return 0;
}

/* add seats present in seats_csv to show's booked_csv (avoiding duplicates). Returns number of seats newly booked. */
int book_seats_for_show(int show_id, const char *seats_csv) {
    Show s; if (!read_show_by_id(show_id, &s)) return 0;
    /* validate seats */
    char tmp[4096]; strncpy(tmp,seats_csv,sizeof(tmp)-1); tmp[sizeof(tmp)-1]=0;
    char *tok = strtok(tmp, ","); int newly=0;
    while(tok) {
        char *seat = ltrim(tok);
        if (!seat_to_rc(seat, NULL, NULL, s.rows, s.cols)) { tok = strtok(NULL, ","); continue; }
        if (!seat_in_csv(s.booked_csv, seat)) {
            if (strlen(s.booked_csv) > 0) strncat(s.booked_csv, ",", sizeof(s.booked_csv)-strlen(s.booked_csv)-1);
            strncat(s.booked_csv, seat, sizeof(s.booked_csv)-strlen(s.booked_csv)-1);
            newly++;
        }
        tok = strtok(NULL, ",");
    }
    if (newly>0) {
        if (!update_show(&s)) return 0;
    }
    return newly;
}

/* cancel seats (remove them from CSV). Returns number of seats removed. */
int cancel_seats_for_show(int show_id, const char *seats_csv) {
    Show s; if(!read_show_by_id(show_id,&s)) return 0;
    /* Build a set of seats to cancel */
    char to_cancel[1024][16]; int tc = 0;
    char tmp[4096]; strncpy(tmp,seats_csv,sizeof(tmp)-1); tmp[sizeof(tmp)-1]=0; char *tok = strtok(tmp, ",");
    while(tok && tc < 1024) { strncpy(to_cancel[tc++], ltrim(tok), 15); to_cancel[tc-1][15]=0; tok = strtok(NULL, ","); }
    if (tc==0) return 0;
    /* build a new booked csv skipping canceled seats */
    char newcsv[4096]; newcsv[0]=0; int removed=0;
    char tmp2[4096]; strncpy(tmp2, s.booked_csv, sizeof(tmp2)-1); tmp2[sizeof(tmp2)-1]=0; char *tok2 = strtok(tmp2, ",");
    while(tok2) {
        char *seat = ltrim(tok2); int should = 0; for(int i=0;i<tc;i++) if (strcmp(seat, to_cancel[i])==0) { should=1; break; }
        if (!should) {
            if (strlen(newcsv)>0) strncat(newcsv, ",", sizeof(newcsv)-strlen(newcsv)-1);
            strncat(newcsv, seat, sizeof(newcsv)-strlen(newcsv)-1);
        } else removed++;
        tok2 = strtok(NULL, ",");
    }
    if (removed>0) { strncpy(s.booked_csv, newcsv, sizeof(s.booked_csv)-1); update_show(&s); }
    return removed;
}

void list_shows() {
    Show *arr; size_t n; if(!read_all_shows(&arr,&n)) { printf("No shows.\n"); return; }
    printf("\nShows:\n");
    for(size_t i=0;i<n;i++) {
        Movie mv; Theatre th; char mname[NAME_LEN], tname[NAME_LEN];
        if(!read_movie_by_id(arr[i].movie_id, &mv)) strncpy(mname, "<unknown>", NAME_LEN);
        else strncpy(mname, mv.name, NAME_LEN);
        if(!read_theatre_by_id(arr[i].theatre_id, &th)) strncpy(tname, "<unknown>", NAME_LEN);
        else strncpy(tname, th.name, NAME_LEN);
        printf("Show id=%d: %s @ %s time %s seats %dx%d booked:%s\n", arr[i].id, mname, tname, arr[i].time, arr[i].rows, arr[i].cols, arr[i].booked_csv[0]?arr[i].booked_csv:" <none>");
    }
    free(arr);
}

int get_show_count() { Show *a; size_t n; if(!read_all_shows(&a,&n)) return 0; free(a); return (int)n; }
int get_show_id_by_index(int idx) { Show *a; size_t n; if(!read_all_shows(&a,&n)) return -1; if (idx<1 || idx> (int)n) { free(a); return -1; } int id=a[idx-1].id; free(a); return id; }
void get_show_dimensions(int show_id, int *rows, int *cols) { Show s; *rows=*cols=0; if(read_show_by_id(show_id,&s)){ *rows=s.rows; *cols=s.cols; } }
void get_show_info(int show_id, int *movie_idx, int *theatre_idx, char *timebuf, int timebuflen) { Show s; *movie_idx = *theatre_idx = 0; if (read_show_by_id(show_id,&s)) { *movie_idx = s.movie_id; *theatre_idx = s.theatre_id; strncpy(timebuf, s.time, timebuflen-1); timebuf[timebuflen-1]=0; } }
int is_seat_booked(int show_id, const char *seat) { Show s; if(!read_show_by_id(show_id,&s)) return 0; return seat_in_csv(s.booked_csv, seat); }

/* admin_add_show - uses storage append */
void admin_add_show() {
    int mcount = get_movie_count(); int tcount = get_theatre_count();
    if (mcount==0 || tcount==0) { printf("Need at least one movie and theatre.\n"); return; }
    printf("Select movie:\n"); list_movies(); printf("Enter movie index (1..): "); int midx; if(scanf("%d", &midx)!=1) { while(getchar()!='\n'); return; }
    int mid = get_movie_id_by_index(midx); if (mid<1) { printf("Invalid movie index.\n"); return; }
    printf("Select theatre:\n"); list_theatres(); printf("Enter theatre index (1..): "); int tidx; if(scanf("%d", &tidx)!=1){ while(getchar()!='\n'); return; }
    int tid = get_theatre_id_by_index(tidx); if (tid<1) { printf("Invalid theatre index.\n"); return; }
    while(getchar()!='\n'); char time[TIME_LEN]; printf("Enter show time (e.g. 18:30): "); fgets(time, sizeof(time), stdin); time[strcspn(time, "\n")] = 0;
    int rows, cols; printf("Rows: "); if(scanf("%d", &rows)!=1) return; printf("Cols: "); if(scanf("%d", &cols)!=1) return;
    Show s = {0}; s.id = next_show_id(); s.movie_id = mid; s.theatre_id = tid; strncpy(s.time, time, TIME_LEN-1); s.rows = rows; s.cols = cols; s.booked_csv[0]=0;
    if (append_show(&s)) printf("Show added id=%d\n", s.id); else printf("Failed to add show\n");
}

/* ===== src/user.c ===== */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/user.h"
#include "../include/booking.h"

/* Users are stored as "username password\n" in a simple text file for demonstration only */
void register_user() {
    char name[64], pass[64]; printf("\nEnter username: "); scanf("%63s", name); printf("Enter password: "); scanf("%63s", pass);
    FILE *f = fopen("data/users.txt", "a"); if(!f) { printf("Failed to open users file.\n"); return; }
    fprintf(f, "%s %s\n", name, pass); fclose(f); printf("Registration successful!\n");
}

void login_user() {
    char name[64], pass[64], u[64], p[64]; printf("\nUsername: "); scanf("%63s", name); printf("Password: "); scanf("%63s", pass);
    FILE *f = fopen("data/users.txt", "r"); int found = 0; if(f){ while(fscanf(f, "%63s %63s", u, p) == 2) { if (strcmp(u, name)==0 && strcmp(p, pass)==0) { found = 1; break; } } fclose(f); }
    if (found) { printf("Login successful!\n"); user_dashboard(name); } else printf("Invalid credentials\n");
}

/* ===== src/booking.c ===== */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "../include/booking.h"
#include "../include/show.h"
#include "../include/movie.h"
#include "../include/theatre.h"

#define MAX_SEAT_INPUT 512

static void spinner_animation(const char *prefix, int cycles) {
    const char *sp = "|/-\\";
    printf("%s ", prefix);
    for (int i=0;i<cycles;i++) { printf("%c\r", sp[i%4]); fflush(stdout); usleep(80000); }
    printf(" ") ;
}

static void print_seat_grid(int rows, int cols, int show_id) {
    printf("\n     SCREEN THIS WAY\n");
    for (int r = 0; r < rows; r++) {
        printf(" %c  ", 'A' + r);
        for (int c = 0; c < cols; c++) {
            char seat[16]; snprintf(seat, sizeof(seat), "%c%d", 'A'+r, c+1);
            int booked = is_seat_booked(show_id, seat);
            printf("[%c]", booked ? 'X' : 'O');
        }
        printf("\n");
    }
    printf("\nO=Available, X=Booked\n");
}

/* parse multi-seat input like "A1,A2,B3" and normalize */
static void normalize_seat_csv(const char *in, char *out, int outlen) {
    out[0]=0; char tmp[MAX_SEAT_INPUT]; strncpy(tmp, in, sizeof(tmp)-1); tmp[sizeof(tmp)-1]=0;
    char *tok = strtok(tmp, ","); int first=1;
    while(tok) {
        while(*tok == ' ' || *tok == '\t') tok++;
        if (strlen(tok)>0) {
            if (!first) strncat(out, ",", outlen-strlen(out)-1);
            strncat(out, tok, outlen-strlen(out)-1);
            first = 0;
        }
        tok = strtok(NULL, ",");
    }
}

void user_dashboard(const char *username) {
    while(1) {
        printf("\n■■■■■■■■■ USER DASHBOARD ■■■■■■■■■\n");
        printf("1. View Movies\n2. View Theatres\n3. View Shows\n4. Book Seats\n5. Cancel Seats\n6. Logout\nChoice: ");
        int ch; if(scanf("%d", &ch)!=1) { while(getchar()!='\n'); continue; }
        if (ch==1) list_movies();
        else if (ch==2) list_theatres();
        else if (ch==3) list_shows();
        else if (ch==4) {
            list_shows(); printf("Enter show id to book: "); int sid; if(scanf("%d", &sid)!=1) { while(getchar()!='\n'); continue; }
            int rows, cols; get_show_dimensions(sid, &rows, &cols); if (rows<=0||cols<=0) { printf("Invalid show id.\n"); continue; }
            print_seat_grid(rows, cols, sid);
            printf("Enter seats to book (comma separated, e.g. A1,A2): "); char buf[512]; while(getchar()!='\n'); fgets(buf, sizeof(buf), stdin); buf[strcspn(buf, "\n")] = 0;
            char seats[512]; normalize_seat_csv(buf, seats, sizeof(seats)); if (strlen(seats)==0) { printf("No seats given.\n"); continue; }
            /* quick seat animation */
            spinner_animation("Booking", 8);
            int newly = book_seats_for_show(sid, seats);
            if (newly>0) {
                /* log booking */
                FILE *f = fopen("data/bookings.txt","a"); if (f) { fprintf(f, "%d|%s|%s\n", sid, seats, username); fclose(f); }
                printf("Booked %d new seats: %s\n", newly, seats);
            } else printf("No seats booked — they may already be reserved or input invalid.\n");
        }
        else if (ch==5) {
            list_shows(); printf("Enter show id to cancel from: "); int sid; if(scanf("%d", &sid)!=1) { while(getchar()!='\n'); continue; }
            int rows, cols; get_show_dimensions(sid, &rows, &cols); if (rows<=0||cols<=0) { printf("Invalid show id.\n"); continue; }
            print_seat_grid(rows, cols, sid);
            printf("Enter seats to cancel (comma separated, e.g. A1,A2): "); char buf[512]; while(getchar()!='\n'); fgets(buf, sizeof(buf), stdin); buf[strcspn(buf, "\n")] = 0;
            char seats[512]; normalize_seat_csv(buf, seats, sizeof(seats)); if (strlen(seats)==0) { printf("No seats given.\n"); continue; }
            spinner_animation("Cancelling", 6);
            int removed = cancel_seats_for_show(sid, seats);
            if (removed>0) {
                /* log cancellation */
                FILE *f = fopen("data/cancellations.txt","a"); if (f) { fprintf(f, "%d|%s|%s\n", sid, seats, username); fclose(f); }
                printf("Cancelled %d seats: %s\n", removed, seats);
            } else printf("No seats cancelled (they may not have been booked).\n");
        }
        else break;
    }
}

/* ===== src/main.c ===== */
#include <stdio.h>
#include <stdlib.h>
#include "../include/user.h"
#include "../include/movie.h"
#include "../include/theatre.h"
#include "../include/show.h"

static void draw_main() {
    printf("\n■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n");
    printf("■    ■ BOOK MY SHOW ■    ■\n");
    printf("■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n");
}

int main() {
    while(1) {
        draw_main();
        printf("1. Register\n2. Login\n3. Admin Menu\n4. Exit\nChoice: ");
        int c; if (scanf("%d", &c)!=1) { while(getchar()!='\n'); continue; }
        if (c==1) register_user();
        else if (c==2) login_user();
        else if (c==3) {
            while(1) {
                printf("\n--- ADMIN ---\n1.Add Movie\n2.Add Theatre\n3.Add Show\n4.List Movies\n5.List Theatres\n6.List Shows\n7.Back\nChoice: ");
                int a; if (scanf("%d", &a)!=1) { while(getchar()!='\n'); break; }
                if (a==1) admin_add_movie();
                else if (a==2) admin_add_theatre();
                else if (a==3) admin_add_show();
                else if (a==4) list_movies();
                else if (a==5) list_theatres();
                else if (a==6) list_shows();
                else break;
            }
        } else break;
    }
    printf("Goodbye!\n"); return 0;
}

/* ===== Makefile =====
CC = gcc
CFLAGS = -std=c99 -Wall -Iinclude
SRC = src/main.c src/user.c src/movie.c src/theatre.c src/show.c src/booking.c src/storage.c
OBJ = $(SRC:.c=.o)
TARGET = app
all: $(TARGET)
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f $(OBJ) $(TARGET)

Notes:
 - Ensure the include headers are in include/ and source files in src/
 - data/ directory will be created automatically (POSIX). On Windows create it manually.
 - This rewrite focuses on clarity and struct-based runtime; further improvements can add concurrency locks, better persistence, and login sessions.
*/
