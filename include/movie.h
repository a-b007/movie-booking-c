#ifndef MOVIE_H
#define MOVIE_H

void admin_add_movie();
void list_movies();
int get_movie_count();
int get_movie_id_by_index(int idx);
char *get_movie_name_by_id(int id, char *buf, int buflen);

#endif