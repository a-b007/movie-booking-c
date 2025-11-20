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
