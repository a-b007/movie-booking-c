#ifndef THEATRE_H
#define THEATRE_H

void admin_add_theatre();
void list_theatres();
int get_theatre_count();
int get_theatre_id_by_index(int idx);
char *get_theatre_name_by_id(int id, char *buf, int buflen);

#endif