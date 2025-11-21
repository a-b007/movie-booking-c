#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/types.h"
#include "../include/theatre.h"

int append_theatre(const Theatre *t);
int read_all_theatres(Theatre **out, size_t *count);
int next_theatre_id();
int read_theatre_by_id(int id, Theatre *dest);

void admin_add_theatre() {
    char name[NAME_LEN], loc[NAME_LEN]; while(getchar()!='\n');
    printf("Enter theatre name: "); fgets(name, sizeof(name), stdin); name[strcspn(name, "\n")] = 0;
    printf("Enter location: "); fgets(loc, sizeof(loc), stdin); loc[strcspn(loc, "\n")] = 0;
    if (strlen(name)==0) { printf("Name empty, abort.\n"); return; }
    Theatre t = {0}; t.id = next_theatre_id(); strncpy(t.name, name, NAME_LEN-1); strncpy(t.location, loc, NAME_LEN-1);
    if (append_theatre(&t)) printf("Added theatre %s (%s) id=%d\n", t.name, t.location, t.id);
    else printf("Failed to add theatre\n");
}

void list_theatres() {
    Theatre *arr; size_t n; if(!read_all_theatres(&arr,&n)) { printf("No theatres available.\n"); return; }
    printf("\nAvailable Theatres:\n"); for(size_t i=0;i<n;i++) printf("%zu. %s (%s) id=%d\n", i+1, arr[i].name, arr[i].location, arr[i].id);
    free(arr);
}

int get_theatre_count() { Theatre *a; size_t n; if(!read_all_theatres(&a,&n)) return 0; free(a); return (int)n; }
int get_theatre_id_by_index(int idx) { Theatre *a; size_t n; if(!read_all_theatres(&a,&n)) return -1; if (idx<1 || idx> (int)n) { free(a); return -1; } int id=a[idx-1].id; free(a); return id; }
char *get_theatre_name_by_id(int id, char *buf, int buflen) { Theatre t; if(!read_theatre_by_id(id, &t)) { buf[0]=0; return buf; } strncpy(buf, t.name, buflen-1); buf[buflen-1]=0; return buf; }
