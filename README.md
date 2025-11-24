# BOOK MY SHOW - C Movie Booking System

By Team Top C - Mohit (BC2025063), Charan (BC2025032), Anamitra (BC2025008)

## Description

This is a complete console-based movie booking management system written in C. It uses structs for clean data representation and binary files for persistent storage. The project allows both administrators and users to interact with a simulated booking environment, including adding movies, theatres, and shows, as well as multi-seat booking and cancellation features.

The interface includes minimal ASCII animation and optional colorized output to improve the user experience in the terminal.

## Features

- Struct-based design for Movies, Theatres, and Shows
- Binary file storage for movies, theatres, and shows
- Text-based user database with login and registration
- Multi-seat booking with seat grid visualization
- Seat cancellation functionality
- Simple admin interface for adding and listing entities
- Terminal animations and color-coded seat display
- Input validation for seat selection

## Installation

1. Download this zip-file:

   ```bash
   movie-booking-c.zip
   ```

2. Build the program using `make`:

   ```bash
   make
   ```

3. Run the executable:

   ```bash
   ./app
   ```



## Usage

1. Run the program.
2. Choose one of the following:
   - **Register**: Create a new user account.
   - **Login**: Access the user dashboard.
   - **Admin Menu**: Access administrative options (protected by a password).
3. In the **Admin Menu**, you can:
   - Add movies
   - Add theatres
   - Add shows
   - List all movies, theatres, and shows
4. In the **User Dashboard**, you can:
   - View movies, theatres, and shows
   - Book one or multiple seats (shows seat map)
   - Cancel booked seats

## Directory Structure

```
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

```
admin123
```

You can change this in `src/main.c`.

## Notes

- Ensure that the terminal supports ANSI escape sequences for colored output.

- Example Output

```
+--+ BOOK MY SHOW +--+
1. Register
2. Login
3. Admin Menu
4. Exit
Choice: 2

Username: test
Password: ****
Login successful!

+--+ USER DASHBOARD +--+
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
```

