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
int get_movie_count();
void list_movies();
int get_movie_id_by_index(int idx);

int get_theatre_count();
void list_theatres();
int get_theatre_id_by_index(int idx);


/* Helper: normalize seat token (remove leading spaces) */
static char *ltrim(char *s) { while (*s==' '||*s=='\t') s++; return s; }

/* check seat validity A1.. */
static int seat_to_rc(const char *seat, int *out_r, int *out_c, int rows, int cols) {
    if (!seat || strlen(seat)<2) return 0;
    char rowch = seat[0]; if (!isalpha((unsigned char)rowch)) return 0;
    int r = toupper(rowch) - 'A'; int c = atoi(seat+1) - 1; if (r<0 || r>=rows || c<0 || c>=cols) return 0;
    if (out_r) *out_r = r;
    if (out_c) *out_c = c;
    return 1;

}

/* split CSV into tokens, calling func for each token */
static void for_each_seat_token(const char *csv, void (*fn)(const char *, void *), void *ud) {
    if (!csv) return;
    char tmp[4096];
    strncpy(tmp, csv, sizeof(tmp)-1);
    tmp[sizeof(tmp)-1]=0;
    char *tok = strtok(tmp, ","); while(tok) { char *s = ltrim(tok); fn(s, ud); tok = strtok(NULL, ","); }
}

static int seat_in_csv(const char *csv, const char *seat) {
    if (!csv || csv[0] == 0) return 0;

    char tmp[4096];
    strncpy(tmp, csv, sizeof(tmp)-1);
    tmp[sizeof(tmp)-1] = 0;

    char *tok = strtok(tmp, ",");
    while(tok) { char *s=ltrim(tok); if (strcmp(s, seat)==0) return 1; tok = strtok(NULL, ","); }
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
    while (getchar() != '\n') {}

    char time[TIME_LEN];
    printf("Enter show time (e.g. 18:30): ");   
    fgets(time, sizeof(time), stdin);
    time[strcspn(time, "\n")] = 0;

    int rows, cols; printf("Rows: "); if(scanf("%d", &rows)!=1) return; printf("Cols: "); if(scanf("%d", &cols)!=1) return;
    Show s = {0}; s.id = next_show_id(); s.movie_id = mid; s.theatre_id = tid; strncpy(s.time, time, TIME_LEN-1); s.rows = rows; s.cols = cols; s.booked_csv[0]=0;
    if (append_show(&s)) printf("Show added id=%d\n", s.id); else printf("Failed to add show\n");
}
