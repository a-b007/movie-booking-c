#**BOOK MY SHOW**

A Movie Booking system written in C by Mohit, Charan and Anamitra (Team Top C)
-Mohit - BC2025063
-Charan - BC2025032
-Anamitra - BC2025008 

##**HOW TO COMPILE**
    -Unzip the .zip file and ensure the folder structure is similar to this
        -*data*
        -*include*
            -booking.h
            -movie.h
            -show.h
            -theatre.h
            -types.h
            -user.h
        -*src*
            -booking.c
            -main.c
            -movie.c
            -show.c
            -storage.c
            -theatre.c
            -user.c
        -MakeFile
        -README.md
    -In terminal or wsl (Linux) run make
    -Run ./app

##**ABOUT THE PROJECT**
    The project creates a terminal based movie booking system.
    -It allows an *admin user* login (Password : admin123)
        -The admin can add Movies, Theatre(Location) and Shows(Movie+Theatre+Time)
    -Users can register/login with credentials and book/cancel a ticket to a particular show. 
    -Multiple seat booking is supported aswell as checks to prevent overlapping seat booking.

