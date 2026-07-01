CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic
QUIET = @
TARGET = smudgec
SRC = src/main.c src/scanner.c src/names.c

EXAMPLE = examples/hello.c
OBFUSCATED = out/hello_obfuscated.c
ORIGINAL_BIN = /tmp/hello_original
OBFUSCATED_BIN = /tmp/hello_obfuscated
ORIGINAL_OUT = /tmp/hello_original.txt
OBFUSCATED_OUT = /tmp/hello_obfuscated.txt

LONG_EXAMPLE = tests/fixtures/long_identifier.c
LONG_OBFUSCATED = out/long_identifier_obfuscated.c
LONG_ORIGINAL_BIN = /tmp/long_identifier_original
LONG_OBFUSCATED_BIN = /tmp/long_identifier_obfuscated
LONG_ORIGINAL_OUT = /tmp/long_identifier_original.txt
LONG_OBFUSCATED_OUT = /tmp/long_identifier_obfuscated.txt

SAFETY_EXAMPLE = tests/fixtures/scanner_safety.c
SAFETY_OBFUSCATED = out/scanner_safety_obfuscated.c
SAFETY_ORIGINAL_BIN = /tmp/scanner_safety_original
SAFETY_OBFUSCATED_BIN = /tmp/scanner_safety_obfuscated
SAFETY_ORIGINAL_OUT = /tmp/scanner_safety_original.txt
SAFETY_OBFUSCATED_OUT = /tmp/scanner_safety_obfuscated.txt

all: $(TARGET)

$(TARGET): $(SRC)
	$(QUIET)echo "[build] $(TARGET)"
	$(QUIET)$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

test: $(TARGET)

	$(QUIET)# First test

	$(QUIET)echo "[test] hello.c"
	$(QUIET)$(CC) $(CFLAGS) $(EXAMPLE) -o $(ORIGINAL_BIN)
	$(QUIET)$(ORIGINAL_BIN) > $(ORIGINAL_OUT)
	$(QUIET)./$(TARGET) $(EXAMPLE) > /tmp/smudgec_hello_tool.txt
	$(QUIET)$(CC) $(CFLAGS) $(OBFUSCATED) -o $(OBFUSCATED_BIN)
	$(QUIET)$(OBFUSCATED_BIN) > $(OBFUSCATED_OUT)
	$(QUIET)diff $(ORIGINAL_OUT) $(OBFUSCATED_OUT)
	$(QUIET)grep -q _sm $(OBFUSCATED)
	$(QUIET)echo "  passed"

	$(QUIET)# Long identifier test

	$(QUIET)echo "[test] long_identifier.c"
	$(QUIET)$(CC) $(CFLAGS) $(LONG_EXAMPLE) -o $(LONG_ORIGINAL_BIN)
	$(QUIET)$(LONG_ORIGINAL_BIN) > $(LONG_ORIGINAL_OUT)
	$(QUIET)./$(TARGET) $(LONG_EXAMPLE) > /tmp/smudgec_long_identifier_tool.txt
	$(QUIET)$(CC) $(CFLAGS) $(LONG_OBFUSCATED) -o $(LONG_OBFUSCATED_BIN)
	$(QUIET)$(LONG_OBFUSCATED_BIN) > $(LONG_OBFUSCATED_OUT)
	$(QUIET)diff $(LONG_ORIGINAL_OUT) $(LONG_OBFUSCATED_OUT)
	$(QUIET)grep -q _sm $(LONG_OBFUSCATED)
	$(QUIET)echo "  passed"

	$(QUIET)# String test 

	$(QUIET)echo "[test] scanner_safety.c"
	$(QUIET)$(CC) $(CFLAGS) $(SAFETY_EXAMPLE) -o $(SAFETY_ORIGINAL_BIN)
	$(QUIET)$(SAFETY_ORIGINAL_BIN) > $(SAFETY_ORIGINAL_OUT)
	$(QUIET)./$(TARGET) $(SAFETY_EXAMPLE) > /tmp/smudgec_scanner_safety_tool.txt
	$(QUIET)$(CC) $(CFLAGS) $(SAFETY_OBFUSCATED) -o $(SAFETY_OBFUSCATED_BIN)
	$(QUIET)$(SAFETY_OBFUSCATED_BIN) > $(SAFETY_OBFUSCATED_OUT)
	$(QUIET)diff $(SAFETY_ORIGINAL_OUT) $(SAFETY_OBFUSCATED_OUT)
	$(QUIET)grep -q "visible_name inside a string" $(SAFETY_OBFUSCATED)
	$(QUIET)grep -q "visible_name inside a line comment" $(SAFETY_OBFUSCATED)
	$(QUIET)grep -q "visible_name inside a block comment" $(SAFETY_OBFUSCATED)
	$(QUIET)grep -q _sm $(SAFETY_OBFUSCATED)
	$(QUIET)echo "  passed"

	$(QUIET)echo "[ok] all tests passed"

clean:
	rm -f $(TARGET)