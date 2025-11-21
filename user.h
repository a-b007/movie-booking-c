#ifndef USER_H
#define USER_H

#include "types.h"

//Struct for storing user data after login
typedef struct {
    id_t id;
    char username[MAX_NAME];
    char password[MAX_NAME]; 
} User;

void user_init_store(void); 
User* user_register(const char *username, const char *password);
User* user_login(const char *username, const char *password);
User* user_find_by_id(id_t id);
void user_save_all(void);
void user_load_all(void);
void user_list(void);
int user_count(void);

#endif 
