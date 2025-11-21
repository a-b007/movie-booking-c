#include "user.h"
#include <stdio.h>
#include <string.h>

//Stays across function calls for persistent storage.
static User users[MAX_USERS];                         //static struct that stays across function calls
static int users_used = 0;                            //count of users registered
static id_t next_user_id = 1;                         //next_id
static const char *USER_FILE = "users.txt";           //file to save relevant data

void user_init_store(void) {
    users_used = 0;
}

User* user_register(const char *username, const char *password) {
    // check if user already exists
    for (int i=0;i<users_used;++i)
        if (strcmp(users[i].username, username)==0)
            return NULL;

    if (users_used >= MAX_USERS) return NULL; //Prevents user from registering if max number of accounts already exist

    User *u = &users[users_used++];           //pointer u to struct User 
    u->id = next_user_id++;                   //Adds the user and increments id

    //Copies username and password
    strncpy(u->username, username, MAX_NAME-1); u->username[MAX_NAME-1]=0; 
    strncpy(u->password, password, MAX_NAME-1); u->password[MAX_NAME-1]=0;
    return u;
}

//LOGIN function: Matches username and password input with stored data in user.txt
User* user_login(const char *username, const char *password) 
{
    for (int i=0;i<users_used;++i) {
        if (strcmp(users[i].username, username)==0 && strcmp(users[i].password, password)==0) {
            return &users[i];
        }
    }
    return NULL;
}

//function to search use via id
User* user_find_by_id(id_t id) {
    for (int i=0;i<users_used;++i) if (users[i].id == id) return &users[i];
    return NULL;
}

//writes id, username and password to a .txt file
void user_save_all(void) {
    FILE *f = fopen(USER_FILE, "w");
    if (!f) return;
    for (int i=0;i<users_used;++i) {
        User *u = &users[i];
        fprintf(f, "%u\t%s\t%s\n", u->id, u->username, u->password);
    }
    fclose(f);
}

//Opens users.txt and reads id username password
void user_load_all(void) {
    FILE *f = fopen(USER_FILE, "r");
    if (!f) return;
    user_init_store();
    unsigned int id; char uname[MAX_NAME], pass[MAX_NAME];
    while (fscanf(f, "%u\t%127[^\t]\t%127[^\n]\n", &id, uname, pass) == 3) {
        if (users_used >= MAX_USERS) break;
        User *u = &users[users_used++];
        u->id = id;
        strncpy(u->username, uname, MAX_NAME-1); u->username[MAX_NAME-1]=0;
        strncpy(u->password, pass, MAX_NAME-1); u->password[MAX_NAME-1]=0;
        if (id >= next_user_id) next_user_id = id + 1;
    }
    fclose(f);
}

//prints id and username
void user_list(void) {
    if (users_used==0) { puts("No users."); return; }
    for (int i=0;i<users_used;++i) {
        printf("[U%u] %s\n", users[i].id, users[i].username);
    }
}


int user_count(void) { return users_used; }
