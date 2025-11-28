# SHOWTIME - C Movie Booking System
   

By Team Top C - Mohit (BC2025063), Charan (BC2025032), Anamitra (BC2025008)
Description

This is a complete console-based movie booking management system written in C. It uses structs for clean data representation and binary files for persistent storage. The project allows both administrators and users to interact with a simulated booking environment, including adding movies, theatres, and shows, as well as multi-seat booking and cancellation features.

The interface includes minimal ASCII animation and optional colorized output to improve the user experience in the terminal.
Features

    -Struct-based design for Movies, Theatres, and Shows

    -Binary file storage for movies, theatres, and shows

    -Text-based user database with login and registration

    -Multi-seat booking with seat grid visualization

    -Seat cancellation functionality

    -Simple admin interface for adding and listing entities

    -Terminal animations and color-coded seat display

    -Input validation for seat selection

## Installation

Download this zip-file:

    movie-booking-c.zip

Build the program using make:

```bash
make
```

Run the executable:

    ./app

## Usage

    Run the program.

    Choose one of the following:

       - Register: Create a new user account.

       - Login: Access the user dashboard.

       - Admin Menu: Access administrative options (protected by a password).

    In the Admin Menu, you can:

       - Add movies

       - Add theatres

       - Add shows

       - List all movies, theatres, and shows

    In the User Dashboard, you can:

        -View movies, theatres, and shows

        -Book one or multiple seats (shows seat map)

        -Cancel booked seats

        -Press ENTER to return to menu(s).

Directory Structure

```bash
project_root/
├── include/
│   ├── types.h
│   ├── movie.h
│   ├── theatre.h
│   ├── show.h
│   ├── user.h
│   └── booking.h
├── src/
│   ├── main.c
│   ├── movie.c
│   ├── theatre.c
│   ├── show.c
│   ├── user.c
│   ├── booking.c
│   └── storage.c
├── data/
├── Makefile
└── README.md
```

## Admin Access

The admin menu is password-protected. The default password is hardcoded as:

```bash
admin123
```

You can change this in src/main.c.
File and Function Reference
src/main.c

Handles program startup and menu navigation.

    main() – Entry point of the program. Manages main menu options (Register, Login, Admin Menu, Exit).

    draw_main() – Prints the program title header.

    Contains logic for switching between admin and user workflows.

src/user.c

Manages user registration and authentication.

    register_user() – Prompts for a username and password, then saves them in data/users.txt.

    login_user() – Validates the entered credentials and, if correct, launches the user dashboard.

src/movie.c

Handles movie-related data and file operations.

    admin_add_movie() – Adds a new movie entry to data/movies.bin.

    list_movies() – Displays all available movies.

    get_movie_count() – Returns the total number of movies.

    get_movie_id_by_index() – Maps a listed movie’s index to its ID.

    get_movie_name_by_id() – Retrieves a movie name by ID for display.

src/theatre.c

Manages theatre information and data storage.

    admin_add_theatre() – Adds a theatre with its name and location to data/theatres.bin.

    list_theatres() – Displays theatres with their names and locations.

    get_theatre_count() – Returns total number of theatres.

    get_theatre_id_by_index() – Maps list index to theatre ID.

src/show.c

Connects movies and theatres via shows and manages seating.

    admin_add_show() – Creates a new show entry combining movie, theatre, time, and seating layout.

    list_shows() – Displays all current shows with their details.

    is_seat_booked() – Checks whether a seat (e.g. A1) is booked.

    book_seats_for_show() – Books one or multiple seats for a given show.

    cancel_seats_for_show() – Cancels one or multiple previously booked seats.

    seat_in_csv() – Helper that checks if a seat is present in a CSV list.

    read_show_by_id() / update_show() – Reads and updates show records in data/shows.bin.

src/booking.c

Implements the user dashboard and seat visualization system.

    user_dashboard(const char *username) – Main user interface that allows logged-in users to view, book, and cancel seats.

    print_seat_grid() – Displays the seating arrangement with colors:

        Green = Available

        Yellow = Booked by current user

        Red = Booked by others

    normalize_seat_csv() – Cleans input strings like A1, A2, B3 into a proper CSV format (A1,A2,B3).

src/storage.c

Provides low-level file storage and retrieval for persistent data.

    append_movie(), append_theatre(), append_show() – Appends a new record to the respective binary files.

    read_all_*() – Loads all records of a type from their files.

    update_show() – Safely updates existing show records.

    ensure_data_dir() – Creates the data directory if it doesn’t exist.

    next_*_id() – Generates sequential unique IDs for each entity.

Notes

    Ensure that the terminal supports ANSI escape sequences for colored output.

    Example Output:

■■ SHOWTIME ■■
1. Register
2. Login
3. Admin Menu
4. Exit
Choice: 2

Username: test
Password: ****
Login successful!

■■ USER DASHBOARD ■■
1. View Movies
2. View Theatres
3. View Shows
4. Book Seats
5. Cancel Seats
6. Logout
Choice: 4

Enter show ID to book: 1

Enter seats (comma separated): A1,A2
Booked 2 new seats.
   
