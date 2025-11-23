#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include "../include/types.h"
#include "../include/movie.h"

int read_movie_by_id(int id, Movie *dest);


/* storage helpers declared in storage.c (forward) */
int append_movie(const Movie *m);
int read_all_movies(Movie **out, size_t *count);
int next_movie_id();

//-----Duplicate check---------
static int movie_exists(const char *name) {
    Movie *arr; size_t n;
    if (!read_all_movies(&arr, &n))
        return 0;

    for (size_t i = 0; i < n; i++) {
        if (strcasecmp(arr[i].name, name) == 0) {
            free(arr);
            return 1;
        }
    }

    free(arr);
    return 0;
}

void admin_add_movie() {
    char name[NAME_LEN]; while(getchar()!='\n');
    printf("Enter movie name: "); 
    fgets(name, sizeof(name), stdin); 
    name[strcspn(name, "\n")] = 0;//strcspn here basically sets the \n as \0
    //taking movie name input from user
    if (strlen(name)==0) { 
        printf("Name empty, abort.\n"); 
        return; 
    } //If name is empty it aborts
    if (movie_exists(name)) {
    printf("Movie '%s' already exists. Duplicate not added.\n", name);
    return;
    } //handling duplicate cases
    Movie m = {0}; 
    m.id = next_movie_id(); 
    strncpy(m.name, name, NAME_LEN-1);
    //copies str from name to m.name and id to next_movie_id()
    if (append_movie(&m)) 
        printf("Added movie %s (id=%d)\n", m.name, m.id);
    //appending the movie to exsisting array of movies.
    else 
        printf("Failed to add movie\n");
}

void list_movies() {
    Movie *arr; size_t n; 
    if(!read_all_movies(&arr,&n)) { 
        printf("No movies available.\n"); 
        return; 
    }
    //Here read_all_movies assigns n the no of movies that are there.
    printf("\nAvailable Movies:\n"); 
    for(size_t i=0;i<n;i++) 
        printf("%zu. %s (id=%d)\n", i+1, arr[i].name, arr[i].id);
    //prints the list of movies read from the binary file
    free(arr);
}

int get_movie_count() { 
    Movie *a; size_t n; 
    if(!read_all_movies(&a,&n)) 
        return 0; 
    free(a); 
    return (int)n; }
//gets the number of movies in binary file
int get_movie_id_by_index(int idx) { 
    Movie *a; size_t n; 
    if(!read_all_movies(&a,&n)) 
    return -1; 
    if (idx<1 || idx> (int)n) { 
        free(a); 
        return -1; 
    } 
    int id=a[idx-1].id; 
    free(a); 
    return id; 
}
//defining a function that takes idx as input and returns id(which is the (idx-1)th element in the array of struct movie) if it lies between 1 and n else,it will return -1
char *get_movie_name_by_id(int id, char *buf, int buflen) { 
    Movie m; 
    if(!read_movie_by_id(id, &m)) { 
        buf[0]=0; 
        return buf; 
    } 
    strncpy(buf, m.name, buflen-1); 
    buf[buflen-1]=0; 
    return buf; 
}
//copy the m.name corresponding to the id to buf and return that

