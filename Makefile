CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic

TARGET = smudgec
SRC = src/main.c src/scanner.c src/names.c

EXAMPLE = examples/hello.c
OBFUSCATED = out/hello_obfuscated.c
ORIGINAL_BIN = /tmp/hello_original
OBFUSCATED_BIN = /tmp/hello_obfuscated
ORIGINAL_OUT = /tmp/hello_original.txt
OBFUSCATED_OUT = /tmp/hello_obfuscated.txt

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

test: $(TARGET)
	$(CC) $(CFLAGS) $(EXAMPLE) -o $(ORIGINAL_BIN)
	$(ORIGINAL_BIN) > $(ORIGINAL_OUT)
	./$(TARGET) $(EXAMPLE)
	$(CC) $(CFLAGS) $(OBFUSCATED) -o $(OBFUSCATED_BIN)
	$(OBFUSCATED_BIN) > $(OBFUSCATED_OUT)
	diff $(ORIGINAL_OUT) $(OBFUSCATED_OUT)

clean:
	rm -f $(TARGET)