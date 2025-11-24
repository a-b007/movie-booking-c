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

// Prints a row x column grid
static void print_seat_grid(int rows, int cols, int show_id) {

    //To center the screen this way text
    for (int i = 0; i < ((4*cols)-7)/2; i++) {
        putchar(' ');
    }
    printf("SCREEN THIS WAY\n");

    printf("      ");

    for (int i = 0; i < 4*cols; i++) {                      //For printing the movie screen
        putchar('-');
    }
    printf("\n     ");
    printf("/");
    printf("\033[47m");
    for (int i = 0; i < 4*cols; i++) {
        putchar('_');
    }
    printf("\033[0m");
    printf("\\");
    
    printf("\n");
    printf("\n");


    for (int r = 0; r < rows; r++) {

        printf(" %c  ", 'A' + r);  // Row label (A, B, C...)
        printf("||");
        for (int c = 0; c < cols; c++) {

            char seat[16];                                                 
            snprintf(seat, sizeof(seat), "%c%d", 'A' + r, c + 1);       //Build label like A1, A2, C3, and so on

            int booked = is_seat_booked(show_id, seat);                 //If seat is booked prints [XX]

            if (booked)
                printf("[\033[31mXX\033[0m]");    // Red for booked
            else
                printf("[\033[32m%s\033[0m]", seat); // Green for available
        }
        printf("||");
        printf("\n");
    }

    printf("\n\033[31mXX\033[0m = Booked\n");
}

//Takes comma-separated input for multiple seats, breaks into tokens based on commas and removes spaces and so on
static void normalize_seat_csv(const char *input, char *output, int max_len) {

    int out_pos = 0;
    int writing_token = 0;

    //Loops until it encounters an empty character
    for (int i = 0; input[i] != '\0'; i++) {

        char ch = input[i];

        if (isalnum(ch)) {                     // Only keep A-Z, 0-9
            output[out_pos++] = ch;
            writing_token = 1;                 //writing_token indicates whether to copy the character or not to output
        }
        else if (ch == ',' && writing_token) { // Add comma only between tokens
            output[out_pos++] = ',';
            writing_token = 0;
        }

        if (out_pos >= max_len - 1) 
            break;
    }

    if (out_pos > 0 && output[out_pos - 1] == ',')
        out_pos--;  // remove trailing comma

    output[out_pos] = '\0';                     //Puts a null char at the end
}

void user_dashboard(const char *username) {

    while (1) {

        printf("\n+-------+ USER DASHBOARD +-------+\n");
        printf("1. View Movies\n");
        printf("2. View Theatres\n");
        printf("3. View Shows\n");
        printf("4. Book Seats\n");
        printf("5. Cancel Seats\n");
        printf("6. Logout\n");
        printf("Choice: ");

        int ch;
        if (scanf("%d", &ch) != 1) {            //Makes sure input is taken. Getchar() as failsafe to read extra characters not taken by scanf.
            while (getchar() != '\n');
            continue;
        }

        //View Movies
        if (ch == 1)
            list_movies();

        //View Theatres
        else if (ch == 2)
            list_theatres();

        //View Shows
        else if (ch == 3)
            list_shows();

        //Book Seats
        else if (ch == 4) {

            list_shows();
            printf("Enter show id to book: ");

            int sid;
            if (scanf("%d", &sid) != 1) {
                while (getchar() != '\n');
                continue;
            }

            int rows, cols;
            get_show_dimensions(sid, &rows, &cols);

            if (rows <= 0 || cols <= 0) {
                printf("Invalid show id.\n");
                continue;
            }

            print_seat_grid(rows, cols, sid);

            printf("Enter seats to book (comma separated, e.g. A1,A2)(Press Enter to return): ");

            char buf[512];
            while (getchar() != '\n');
            fgets(buf, sizeof(buf), stdin);
            buf[strcspn(buf, "\n")] = 0;

            char seats[512];
            normalize_seat_csv(buf, seats, sizeof(seats));

            if (strlen(seats) == 0) {
                printf("No seats given.\n");
                continue;
            }

            printf("Booking...\n");

            int newly = book_seats_for_show(sid, seats);

            if (newly > 0) {

                //Applends booking data to bookings.txt
                FILE *f = fopen("data/bookings.txt", "a");
                if (f) {
                    fprintf(f, "%d|%s|%s\n", sid, seats, username);
                    fclose(f);
                }

                printf("Booked %d new seats: %s\n", newly, seats);

            } else {
                printf("No seats booked — they may already be reserved or input invalid.\n");
            }
        }

        //Cancelling Seats
        else if (ch == 5) {

            list_shows();
            printf("Enter show id to cancel from: ");

            int sid;
            if (scanf("%d", &sid) != 1) {
                while (getchar() != '\n');
                continue;
            }

            int rows, cols;
            get_show_dimensions(sid, &rows, &cols);

            if (rows <= 0 || cols <= 0) {
                printf("Invalid show id.\n");
                continue;
            }

            print_seat_grid(rows, cols, sid);

            printf("Enter seats to cancel (comma separated, e.g. A1,A2)(Enter to return): ");

            char buf[512];
            while (getchar() != '\n');
            fgets(buf, sizeof(buf), stdin);
            buf[strcspn(buf, "\n")] = 0;

            char seats[512];
            normalize_seat_csv(buf, seats, sizeof(seats));

            if (strlen(seats) == 0) {
                printf("No seats given.\n");
                continue;
            }

            printf("Cancelling...\n");

            int removed = cancel_seats_for_show(sid, seats);

            if (removed > 0) {

                //Appends to cancellations.txt
                FILE *f = fopen("data/cancellations.txt", "a");
                if (f) {
                    fprintf(f, "%d|%s|%s\n", sid, seats, username);
                    fclose(f);
                }

                printf("Cancelled %d seats: %s\n", removed, seats);

            } else {
                printf("No seats cancelled (they may not have been booked).\n");
            }
        }

        else
            break;
    }
}
