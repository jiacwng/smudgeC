CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic

TARGET = smudgec
SRC = src/main.c src/scanner.c src/names.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)