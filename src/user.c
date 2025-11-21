#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/user.h"
#include "../include/booking.h"

//Users are stored as username and password in users.txt
void register_user()
    {
        char name[64], pass[64];

        printf("\nEnter username: ");
        scanf("%63s", name);

        printf("Enter password: ");
        scanf("%63s", pass);
        FILE *f = fopen("data/users.txt", "a");         //Opens in append mode

        if(!f) { printf("Failed to open users file.\n"); return; }
        
        fprintf(f, "%s %s\n", name, pass);
        fclose(f);

        printf("Registration successful !\n");
    }


void login_user() {

    char name[64], pass[64], u[64], p[64];

    printf("\nUsername: ");                                                    //Gets user input of username and password
    scanf("%63s", name);

    printf("Password: ");
    scanf("%63s", pass);

    FILE *f = fopen("data/users.txt", "r");                                    //Cross-verifies with users.txt
    int found = 0;

    if(f){                                                                     //If fileopened succesfully 
        while(fscanf(f, "%63s %63s", u, p) == 2)                               //Reads from file and stores to u and p
        { 
            if (strcmp(u, name)==0 && strcmp(p, pass)==0)                      //Compares username and password with actual ones from file
            {   found = 1;
                break;
            }
        }
        fclose(f);
     }

    if (found) { 
        printf("Login successful!\n");
        user_dashboard(name);
     }
    else printf("Invalid credentials\n");
}
