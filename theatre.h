#ifndef THEATRE_H
#define THEATRE_H

#include "types.h"

typedef struct {
    id_t id;
    char name[MAX_NAME];
    char city[MAX_NAME];
} Theatre;

void theatre_init_store(void);
Theatre* theatre_create(const char *name, const char *city);
Theatre* theatre_find_by_id(id_t id);
void theatre_list(void);
int theatre_count(void);
void theatre_save_all(void);
void theatre_load_all(void);

#endif // THEATRE_H
