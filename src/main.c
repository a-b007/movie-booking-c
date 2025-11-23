#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/user.h"
#include "../include/movie.h"
#include "../include/theatre.h"
#include "../include/show.h"


static void draw_main() {
    printf("\n--------------------------------------\n");
    printf("+----------+ BOOK MY SHOW +----------+\n");
    printf("--------------------------------------\n");
}

int main() {
    while(1) {                                                                          //Loop around main_menu
        draw_main();
        //Print BOOK MY SHOW
        printf("1. Register\n2. Login\n3. Admin Menu\n4. Exit\nChoice: ");

        int c;
        
        if (scanf("%d", &c)!=1) 
        {
            while(getchar()!='\n');
            continue; 
        }

        if (c==1) register_user();

        else if (c==2) login_user();

        else if (c==3)
        {
            char pass[64];
            printf("Enter admin password: ");
            scanf("%63s",pass);

            if (strcmp(pass, "admin123")!=0)
            {
                printf("Access denied. Incorrect password.\n");
                continue;
            }

            while(1)
             {
                printf("\n--- ADMIN ---\n1.Add Movie\n2.Add Theatre\n3.Add Show\n4.List Movies\n5.List Theatres\n6.List Shows\n7.Back\nChoice: ");

                int a;

                if (scanf("%d", &a)!=1)
                { 
                    while(getchar()!='\n');

                    break;
                }

                if (a==1) admin_add_movie();

                else if (a==2) admin_add_theatre();

                else if (a==3) admin_add_show();

                else if (a==4) list_movies();

                else if (a==5) list_theatres();

                else if (a==6) list_shows();

                else break;
            }
        } 
        else break;
    }
    printf("Goodbye!\n");
    
    return 0;
}
