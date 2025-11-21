# Compiler and flags
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2
DEBUG_FLAGS = -std=c11 -Wall -Wextra -Wpedantic -g -O0

# Project name
TARGET = booking_app

# Source files
SRC = main.c \
      movie.c \
      theatre.c \
      show.c \
      booking.c \
      user.c \
      storage.c

# Headers
HEADERS = types.h movie.h theatre.h show.h booking.h user.h storage.h

# Object files
OBJ = $(SRC:.c=.o)

# Default build
all: $(TARGET)

# Release build
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

# Debug build
debug:
	$(CC) $(DEBUG_FLAGS) $(SRC) -o $(TARGET)

# Compile each .c → .o
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -MMD -c $< -o $@

# Auto-include dependency files
-include $(SRC:.c=.d)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Clean object files and binary
clean:
	rm -f $(OBJ) $(SRC:.c=.d) $(TARGET)

# Full reset
distclean: clean
	rm -f movies.txt theatres.txt shows.txt bookings.txt users.txt

.PHONY: all clean distclean run debug
