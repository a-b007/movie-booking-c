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