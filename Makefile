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

INTS_EXAMPLE = tests/fixtures/ints.c
INTS_OBFUSCATED = out/ints_obfuscated.c
INTS_ORIGINAL_BIN = /tmp/ints_original
INTS_OBFUSCATED_BIN = /tmp/ints_obfuscated
INTS_ORIGINAL_OUT = /tmp/ints_original.txt
INTS_OBFUSCATED_OUT = /tmp/ints_obfuscated.txt

INT_EDGES_EXAMPLE = tests/fixtures/int_edges.c
INT_EDGES_OBFUSCATED = out/int_edges_obfuscated.c
INT_EDGES_ORIGINAL_BIN = /tmp/int_edges_original
INT_EDGES_OBFUSCATED_BIN = /tmp/int_edges_obfuscated
INT_EDGES_ORIGINAL_OUT = /tmp/int_edges_original.txt
INT_EDGES_OBFUSCATED_OUT = /tmp/int_edges_obfuscated.txt

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


	$(QUIET)# windows directory test

	$(QUIET)echo "[test] windows-style path separator"
	$(QUIET)WIN_PATH=/tmp/smudgec_win_path\\hello.c; \
	printf '#include <stdio.h>\nint main(void){printf("win path\\n"); return 0;}\n' > "$$WIN_PATH"; \
	./$(TARGET) "$$WIN_PATH" > /tmp/smudgec_win_path_tool.txt; \
	test -f out/hello_obfuscated.c; \
	$(CC) $(CFLAGS) out/hello_obfuscated.c -o /tmp/smudgec_win_path_bin; \
	/tmp/smudgec_win_path_bin > /tmp/smudgec_win_path_out.txt; \
	grep -q "win path" /tmp/smudgec_win_path_out.txt; \
	rm -f "$$WIN_PATH"
	$(QUIET)echo "  passed"

	$(QUIET)# combined flags test

	$(QUIET)echo "[test] combined short flags"
	$(QUIET)./$(TARGET) -se $(SAFETY_EXAMPLE) > /tmp/smudgec_short_flags.txt
	$(QUIET)grep -q "wrote: out/scanner_safety_obfuscated.c" /tmp/smudgec_short_flags.txt
	$(QUIET)! grep -q "visible_name inside a line comment" $(SAFETY_OBFUSCATED)
	$(QUIET)grep -q "visible_name inside a string" $(SAFETY_OBFUSCATED)
	$(QUIET)echo "  passed"
	$(QUIET)echo "[test] combined long flags"
	$(QUIET)./$(TARGET) --strip-comments --encode-ints $(SAFETY_EXAMPLE) > /tmp/smudgec_long_flags.txt
	$(QUIET)grep -q "wrote: out/scanner_safety_obfuscated.c" /tmp/smudgec_long_flags.txt
	$(QUIET)! grep -q "visible_name inside a block comment" $(SAFETY_OBFUSCATED)
	$(QUIET)grep -q "visible_name inside a string" $(SAFETY_OBFUSCATED)
	$(QUIET)echo "  passed"

	$(QUIET)# unknown option test

	$(QUIET)echo "[test] reject unknown short option"
	$(QUIET)! ./$(TARGET) -x $(EXAMPLE) > /tmp/smudgec_unknown_short.txt
	$(QUIET)grep -q "unknown option: -x" /tmp/smudgec_unknown_short.txt
	$(QUIET)echo "  passed"
	$(QUIET)echo "[test] reject unknown long option"
	$(QUIET)! ./$(TARGET) --bad $(EXAMPLE) > /tmp/smudgec_unknown_long.txt
	$(QUIET)grep -q "unknown option: --bad" /tmp/smudgec_unknown_long.txt
	$(QUIET)echo "  passed"

	$(QUIET)# help output test

	$(QUIET)echo "[test] help output"
	$(QUIET)./$(TARGET) --help > /tmp/smudgec_help_long.txt
	$(QUIET)./$(TARGET) -h > /tmp/smudgec_help_short.txt
	$(QUIET)grep -q "usage: smudgec" /tmp/smudgec_help_long.txt
	$(QUIET)grep -q "usage: smudgec" /tmp/smudgec_help_short.txt
	$(QUIET)grep -q -- "--strip-comments" /tmp/smudgec_help_long.txt
	$(QUIET)grep -q -- "--encode-ints" /tmp/smudgec_help_short.txt
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


	$(QUIET) # encoded ints test

	$(QUIET)echo "[test] ints.c --encode-ints"
	$(QUIET)$(CC) $(CFLAGS) $(INTS_EXAMPLE) -o $(INTS_ORIGINAL_BIN)
	$(QUIET)$(INTS_ORIGINAL_BIN) > $(INTS_ORIGINAL_OUT)
	$(QUIET)./$(TARGET) -e $(INTS_EXAMPLE) > /tmp/smudgec_ints_tool.txt
	$(QUIET)$(CC) $(CFLAGS) $(INTS_OBFUSCATED) -o $(INTS_OBFUSCATED_BIN)
	$(QUIET)$(INTS_OBFUSCATED_BIN) > $(INTS_OBFUSCATED_OUT)
	$(QUIET)diff $(INTS_ORIGINAL_OUT) $(INTS_OBFUSCATED_OUT)
	$(QUIET)grep -q "((47 ^ 40))" $(INTS_OBFUSCATED)
	$(QUIET)grep -q "((16 ^ 58))" $(INTS_OBFUSCATED)
	$(QUIET)grep -q "((46 ^ 36))" $(INTS_OBFUSCATED)
	$(QUIET)grep -q "return 0;" $(INTS_OBFUSCATED)
	$(QUIET)echo "  passed"

	$(QUIET) # encoded int edge cases test

	$(QUIET)echo "[test] int_edges.c --encode-ints"
	$(QUIET)$(CC) $(CFLAGS) $(INT_EDGES_EXAMPLE) -o $(INT_EDGES_ORIGINAL_BIN)
	$(QUIET)$(INT_EDGES_ORIGINAL_BIN) > $(INT_EDGES_ORIGINAL_OUT)
	$(QUIET)./$(TARGET) -e $(INT_EDGES_EXAMPLE) > /tmp/smudgec_int_edges_tool.txt
	$(QUIET)$(CC) $(CFLAGS) $(INT_EDGES_OBFUSCATED) -o $(INT_EDGES_OBFUSCATED_BIN)
	$(QUIET)$(INT_EDGES_OBFUSCATED_BIN) > $(INT_EDGES_OBFUSCATED_OUT)
	$(QUIET)diff $(INT_EDGES_ORIGINAL_OUT) $(INT_EDGES_OBFUSCATED_OUT)
	$(QUIET)grep -q "42u" $(INT_EDGES_OBFUSCATED)
	$(QUIET)grep -q "3.14" $(INT_EDGES_OBFUSCATED)
	$(QUIET)grep -q "((79 ^ 71))" $(INT_EDGES_OBFUSCATED)
	$(QUIET)grep -q "return 0;" $(INT_EDGES_OBFUSCATED)
	$(QUIET)echo "  passed"

	$(QUIET)echo "[ok] all tests passed"

clean:
	rm -f $(TARGET)
