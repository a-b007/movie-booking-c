#include "theatre.h"
#include <stdio.h>
#include <string.h>

static Theatre theatres[MAX_THEATRES];
static int theatres_used = 0;
static id_t next_theatre_id = 1;
static const char *THEATRE_FILE = "theatres.txt";

void theatre_init_store(void) {
    theatres_used = 0;
}

Theatre* theatre_create(const char *name, const char *city) {
    if (theatres_used >= MAX_THEATRES) return NULL;
    Theatre *t = &theatres[theatres_used++];
    t->id = next_theatre_id++;
    strncpy(t->name, name, MAX_NAME-1); t->name[MAX_NAME-1]=0;
    strncpy(t->city, city, MAX_NAME-1); t->city[MAX_NAME-1]=0;
    return t;
}

Theatre* theatre_find_by_id(id_t id) {
    for (int i=0;i<theatres_used;++i) if (theatres[i].id == id) return &theatres[i];
    return NULL;
}

void theatre_list(void) {
    if (theatres_used == 0) { puts("No theatres."); return; }
    for (int i=0;i<theatres_used;++i) {
        Theatre *t = &theatres[i];
        printf("[T%u] %s (%s)\n", t->id, t->name, t->city);
    }
}

int theatre_count(void) { return theatres_used; }

void theatre_save_all(void) {
    FILE *f = fopen(THEATRE_FILE, "w");
    if (!f) return;
    for (int i=0;i<theatres_used;++i) {
        Theatre *t = &theatres[i];
        fprintf(f, "%u\t%s\t%s\n", t->id, t->name, t->city);
    }
    fclose(f);
}

void theatre_load_all(void) {
    FILE *f = fopen(THEATRE_FILE, "r");
    if (!f) return;
    theatre_init_store();
    unsigned int id; char name[MAX_NAME], city[MAX_NAME];
    while (fscanf(f, "%u\t%127[^\t]\t%127[^\n]\n", &id, name, city) == 3) {
        Theatre *t = &theatres[theatres_used++];
        t->id = id;
        strncpy(t->name, name, MAX_NAME-1); t->name[MAX_NAME-1]=0;
        strncpy(t->city, city, MAX_NAME-1); t->city[MAX_NAME-1]=0;
        if (id >= next_theatre_id) next_theatre_id = id + 1;
        if (theatres_used >= MAX_THEATRES) break;
    }
    fclose(f);
}
