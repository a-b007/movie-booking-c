CC = gcc
CFLAGS = -std=c99 -Wall -Iinclude
SRC = src/main.c src/user.c src/movie.c src/theatre.c src/show.c src/booking.c src/storage.c
OBJ = $(SRC:.c=.o)
TARGET = app

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
