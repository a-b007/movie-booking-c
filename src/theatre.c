#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/types.h"
#include "../include/theatre.h"

int append_theatre(const Theatre *t);
int read_all_theatres(Theatre **out, size_t *count);
int next_theatre_id();
int read_theatre_by_id(int id, Theatre *dest);

// ----- Duplicate Check -----
static int theatre_exists(const char *name, const char *loc) {
    Theatre *arr; size_t n;
    if (!read_all_theatres(&arr, &n))
        return 0;

    for (size_t i = 0; i < n; i++) {
        if (strcasecmp(arr[i].name, name) == 0 &&
            strcasecmp(arr[i].location, loc) == 0) {
            free(arr);
            return 1;
        }
    }

    free(arr);
    return 0;
}
void admin_add_theatre() {
    char name[NAME_LEN], loc[NAME_LEN]; while(getchar()!='\n');
    printf("Enter theatre name: "); 
    fgets(name, sizeof(name), stdin); 
    name[strcspn(name, "\n")] = 0;//here strcspn set the \n ans \0
    printf("Enter location: "); 
    fgets(loc, sizeof(loc), stdin); 
    loc[strcspn(loc, "\n")] = 0; //getting theatre location
    if (strlen(name)==0) { 
        printf("Name empty, abort.\n"); 
        return; 
    } //aborting it len(name)==0
    if (theatre_exists(name, loc)) {
    printf("Theatre '%s' at '%s' already exists. Duplicate not added.\n",
           name, loc);
    return;
    } //Handling duplicate cases
    Theatre t = {0}; 
    t.id = next_theatre_id(); 
    strncpy(t.name, name, NAME_LEN-1); 
    strncpy(t.location, loc, NAME_LEN-1); //putting the inputted data into the struct Theatre
    if (append_theatre(&t)) 
        printf("Added theatre %s (%s) id=%d\n", t.name, t.location, t.id); //appending these to the binary file
    else 
        printf("Failed to add theatre\n");
}

void list_theatres() {
    Theatre *arr; size_t n; 
    if(!read_all_theatres(&arr,&n)) { 
        printf("No theatres available.\n"); 
        return; 
    }
    printf("\nAvailable Theatres:\n"); 
    for(size_t i=0;i<n;i++) 
        printf("%zu. %s (%s) id=%d\n", i+1, arr[i].name, arr[i].location, arr[i].id);
    free(arr);
}
//printing the list of theateres

int get_theatre_count() { 
    Theatre *a; size_t n; 
    if(!read_all_theatres(&a,&n)) 
        return 0; 
    free(a); 
    return (int)n; 
}
//getting the count of number of struct Theateres in the binary file
int get_theatre_id_by_index(int idx) { 
    Theatre *a; size_t n; 
    if(!read_all_theatres(&a,&n)) 
    return -1; 
    if (idx<1 || idx> (int)n) { 
        free(a); 
        return -1; 
    } 
    int id=a[idx-1].id; 
    free(a); 
    return id; 
}
//getting the id by inputiing the idx value
char *get_theatre_name_by_id(int id, char *buf, int buflen) { 
    Theatre t; 
    if(!read_theatre_by_id(id, &t)) { 
        buf[0]=0; 
        return buf; 
    } 
    strncpy(buf, t.name, buflen-1); 
    buf[buflen-1]=0; 
    return buf; 
}
//geeting the thatre name by using id