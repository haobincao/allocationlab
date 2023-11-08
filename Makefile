SHELL := /bin/bash
CC := gcc
CFLAGS += -Wall -Wextra -std=c17 -MMD -MP -Isrc -m32
LDFLAGS += -lm

# executables with a main
MAIN := src/mtest.c
MAIN_BIN := $(patsubst src/%.c,bin/%,$(MAIN))

# executable tests (must start with "test_")
TEST := $(wildcard test/test_*.c)
TEST_BIN := $(patsubst test/test_%.c,bin/test_%,$(TEST))
TEST_RES := $(patsubst test/test_%.c,test/test_%.res,$(TEST))

BIN := $(MAIN_BIN) $(TEST_BIN)
OBJ := $(patsubst src/%.c,build/%.o,$(wildcard src/*.c)) \
       $(patsubst test/%.c,build/test/%.o,$(wildcard test/*.c))

.PHONY: debug release clean
.DEFAULT_GOAL := debug

# use BIN and OBJ to keep intermediate results

debug: CFLAGS += -Og -g -DDEBUG
debug: $(BIN) $(OBJ)

release: CFLAGS += -O3 -DNDEBUG
release: clean $(BIN) $(OBJ)
	@# so that "make debug" will rebuild .o
	make cleanobj

# build object files
build/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

build/test/%.o: test/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# save them all in a static library
build/liball.a: $(OBJ)
	ar rcs $@ $^

# link test binaries (with needed depedencies)
bin/test_%: build/test/test_%.o build/liball.a
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# link main binaries (with needed depedencies)
bin/%: build/%.o build/liball.a
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# generate test results
test/test_%.res: bin/test_%
	-./$< > $@ 2>&1

# run the tests
test: debug $(TEST_RES)
	@printf "\n=== TESTS\n\n"
	@printf ">>> PASSED\n"
	@printf "`grep -s :PASS test/*.res | sed 's/:/\t/'`\n\n"
	@printf ">>> FAILED\n"
	@printf "`grep -s -P '(:FAIL|Assertion)' test/*.res | sed 's/:/\t/'` \n\n"

# include header dependencies from GCC
-include $(OBJ:.o=.d)

cleanobj:
	rm -f build/*.{a,d,o} build/test/*.{d,o}

clean: cleanobj
	rm -f bin/* test/*.res
