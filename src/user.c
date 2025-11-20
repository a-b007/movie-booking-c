#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/user.h"
#include "../include/booking.h"

/* Users are stored as "username password\n" in a simple text file for demonstration only */
void register_user() {
    char name[64], pass[64]; printf("\nEnter username: "); scanf("%63s", name); printf("Enter password: "); scanf("%63s", pass);
    FILE *f = fopen("data/users.txt", "a"); if(!f) { printf("Failed to open users file.\n"); return; }
    fprintf(f, "%s %s\n", name, pass); fclose(f); printf("Registration successful!\n");
}

void login_user() {
    char name[64], pass[64], u[64], p[64]; printf("\nUsername: "); scanf("%63s", name); printf("Password: "); scanf("%63s", pass);
    FILE *f = fopen("data/users.txt", "r"); int found = 0; if(f){ while(fscanf(f, "%63s %63s", u, p) == 2) { if (strcmp(u, name)==0 && strcmp(p, pass)==0) { found = 1; break; } } fclose(f); }
    if (found) { printf("Login successful!\n"); user_dashboard(name); } else printf("Invalid credentials\n");
}