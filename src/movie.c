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
