#ifndef SHOW_H
#define SHOW_H

void admin_add_show();
void list_shows();
int get_show_count();
int get_show_id_by_index(int idx);
void get_show_dimensions(int show_id, int *rows, int *cols);
void get_show_info(int show_id, int *movie_idx, int *theatre_idx, char *timebuf, int timebuflen);
int is_seat_booked(int show_id, const char *seat);
int book_seats_for_show(int show_id, const char *seats_csv); // seats_csv: "A1,A2" 
int cancel_seats_for_show(int show_id, const char *seats_csv);

#endif