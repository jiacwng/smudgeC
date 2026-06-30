CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic

TARGET = smudgec
SRC = src/main.c src/scanner.c src/names.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)


test: $(TARGET)
	./$(TARGET) examples/hello.c
	gcc out/hello_obfuscated.c -o /tmp/hello_obfuscated
	/tmp/hello_obfuscated


clean:
	rm -f $(TARGET)