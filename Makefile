CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic
QUIET = @
TARGET = smudgec
SRC = src/main.c src/scanner.c src/names.c src/path_utils.c

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

STDLIB_EXAMPLE = tests/fixtures/stdlib.c
STDLIB_OBFUSCATED = out/stdlib_obfuscated.c
STDLIB_ORIGINAL_BIN = /tmp/stdlib_original
STDLIB_OBFUSCATED_BIN = /tmp/stdlib_obfuscated
STDLIB_ORIGINAL_OUT = /tmp/stdlib_original.txt
STDLIB_OBFUSCATED_OUT = /tmp/stdlib_obfuscated.txt

KEYWORDS_EXAMPLE = tests/fixtures/keywords.c
KEYWORDS_OBFUSCATED = out/keywords_obfuscated.c
KEYWORDS_ORIGINAL_BIN = /tmp/keywords_original
KEYWORDS_OBFUSCATED_BIN = /tmp/keywords_obfuscated
KEYWORDS_ORIGINAL_OUT = /tmp/keywords_original.txt
KEYWORDS_OBFUSCATED_OUT = /tmp/keywords_obfuscated.txt

SAFETY_STRIPPED = out/scanner_safety_obfuscated.c
SAFETY_STRIPPED_BIN = /tmp/scanner_safety_stripped
SAFETY_STRIPPED_OUT = /tmp/scanner_safety_stripped.txt

BAD_EXTENSION = README.md
BAD_EXTENSION_OUT = /tmp/smudgec_bad_extension.txt

DIRECTORY_INPUT = examples
DIRECTORY_INPUT_OUT = /tmp/smudgec_directory_input.txt

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

	$(QUIET)# non .c test

	$(QUIET)echo "[test] reject non-.c input"
	$(QUIET)! ./$(TARGET) $(BAD_EXTENSION) > $(BAD_EXTENSION_OUT)
	$(QUIET)grep -q "expected a .c file" $(BAD_EXTENSION_OUT)
	$(QUIET)echo "  passed"

	$(QUIET)# long path test


	$(QUIET)echo "[test] reject too-long output path"
	$(QUIET)LONG_NAME=$$(printf 'a%.0s' $$(seq 1 244)).c; \
	printf 'int main(void){return 0;}\n' > /tmp/$$LONG_NAME; \
	! ./$(TARGET) /tmp/$$LONG_NAME > /tmp/smudgec_long_path.txt; \
	grep -q "output path too long" /tmp/smudgec_long_path.txt; \
	rm -f /tmp/$$LONG_NAME
	$(QUIET)echo "  passed"

	$(QUIET)# reject directory input test

	$(QUIET)echo "[test] reject directory input"
	$(QUIET)! ./$(TARGET) $(DIRECTORY_INPUT) > $(DIRECTORY_INPUT_OUT)
	$(QUIET)grep -q "input path is a directory" $(DIRECTORY_INPUT_OUT)
	$(QUIET)echo "  passed"


	$(QUIET)# blocked output directory test
	
	$(QUIET)echo "[test] reject output path blocked by file"
	$(QUIET)rm -rf out
	$(QUIET)printf 'not a directory\n' > out
	$(QUIET)! ./$(TARGET) $(EXAMPLE) > /tmp/smudgec_out_file.txt
	$(QUIET)grep -q "out exists and is not a directory" /tmp/smudgec_out_file.txt
	$(QUIET)rm -f out
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

	$(QUIET)echo "[test] scanner_safety.c --strip-comments"
	$(QUIET)./$(TARGET) --strip-comments $(SAFETY_EXAMPLE) > /tmp/smudgec_scanner_safety_strip_tool.txt
	$(QUIET)$(CC) $(CFLAGS) $(SAFETY_STRIPPED) -o $(SAFETY_STRIPPED_BIN)
	$(QUIET)$(SAFETY_STRIPPED_BIN) > $(SAFETY_STRIPPED_OUT)
	$(QUIET)diff $(SAFETY_ORIGINAL_OUT) $(SAFETY_STRIPPED_OUT)
	$(QUIET)! grep -q "visible_name inside a line comment" $(SAFETY_STRIPPED)
	$(QUIET)! grep -q "visible_name inside a block comment" $(SAFETY_STRIPPED)
	$(QUIET)grep -q "visible_name inside a string" $(SAFETY_STRIPPED)
	$(QUIET)grep -q _sm $(SAFETY_STRIPPED)
	$(QUIET)echo "  passed"

	$(QUIET) # protected identifiers test

	$(QUIET)echo "[test] stdlib.c"
	$(QUIET)$(CC) $(CFLAGS) $(STDLIB_EXAMPLE) -o $(STDLIB_ORIGINAL_BIN)
	$(QUIET)$(STDLIB_ORIGINAL_BIN) > $(STDLIB_ORIGINAL_OUT)
	$(QUIET)./$(TARGET) $(STDLIB_EXAMPLE) > /tmp/smudgec_stdlib_tool.txt
	$(QUIET)$(CC) $(CFLAGS) $(STDLIB_OBFUSCATED) -o $(STDLIB_OBFUSCATED_BIN)
	$(QUIET)$(STDLIB_OBFUSCATED_BIN) > $(STDLIB_OBFUSCATED_OUT)
	$(QUIET)diff $(STDLIB_ORIGINAL_OUT) $(STDLIB_OBFUSCATED_OUT)
	$(QUIET)grep -q "malloc" $(STDLIB_OBFUSCATED)
	$(QUIET)grep -q "free" $(STDLIB_OBFUSCATED)
	$(QUIET)grep -q "NULL" $(STDLIB_OBFUSCATED)
	$(QUIET)grep -q _sm $(STDLIB_OBFUSCATED)
	$(QUIET)echo "  passed"

	$(QUIET) # keyword test

	$(QUIET)echo "[test] keywords.c"
	$(QUIET)$(CC) $(CFLAGS) $(KEYWORDS_EXAMPLE) -o $(KEYWORDS_ORIGINAL_BIN)
	$(QUIET)$(KEYWORDS_ORIGINAL_BIN) > $(KEYWORDS_ORIGINAL_OUT)
	$(QUIET)./$(TARGET) $(KEYWORDS_EXAMPLE) > /tmp/smudgec_keywords_tool.txt
	$(QUIET)$(CC) $(CFLAGS) $(KEYWORDS_OBFUSCATED) -o $(KEYWORDS_OBFUSCATED_BIN)
	$(QUIET)$(KEYWORDS_OBFUSCATED_BIN) > $(KEYWORDS_OBFUSCATED_OUT)
	$(QUIET)diff $(KEYWORDS_ORIGINAL_OUT) $(KEYWORDS_OBFUSCATED_OUT)
	$(QUIET)grep -q "typedef" $(KEYWORDS_OBFUSCATED)
	$(QUIET)grep -q "struct" $(KEYWORDS_OBFUSCATED)
	$(QUIET)grep -q "switch" $(KEYWORDS_OBFUSCATED)
	$(QUIET)grep -q "sizeof" $(KEYWORDS_OBFUSCATED)
	$(QUIET)grep -q _sm $(KEYWORDS_OBFUSCATED)
	$(QUIET)echo "  passed"

	$(QUIET)echo "[ok] all tests passed"

clean:
	rm -f $(TARGET)