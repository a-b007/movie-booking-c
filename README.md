
BOOK MY SHOW

A Movie Booking System written in C by Team Top C

Contributors:

    Mohit - BC2025063

    Charan - BC2025032

    Anamitra - BC2025008

ABOUT THE PROJECT

This project implements a terminal-based movie booking system in C.

Features:

    Admin login (Password: admin123)

        Add and manage Movies, Theatres (Locations), and Shows (Movie + Theatre + Time).

    User accounts:

        Users can register or log in with credentials.

        Book or cancel tickets for available shows.

        Supports multiple seat bookings.

        Includes validation to prevent overlapping seat reservations.

HOW TO COMPILE AND RUN
Folder Structure

After unzipping the project folder, ensure the structure is as follows:

book-my-show/
|── data/
├── include/
│   ├── booking.h
│   ├── movie.h
│   ├── show.h
│   ├── theatre.h
│   ├── types.h
│   ├── user.h
├── src/
│   ├── booking.c
│   ├── main.c
│   ├── movie.c
│   ├── show.c
│   ├── storage.c
│   ├── theatre.c
│   ├── user.c
├── Makefile
└── README.md

Build Instructions

Open a terminal (Linux or WSL) and run:

make

This will compile all source files and create an executable named app.
Run the Program

./app

NOTES

    Written in C .

    Modular design with separate files for each major component (Movie, Theatre, User, etc.).

    Data persistence is implemented using text-based storage in the data/ folder.

