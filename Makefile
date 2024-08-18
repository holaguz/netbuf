BUILD_DIR = build
TEST_FILES = $(wildcard test/*.cpp)

CC  ?= gcc
CXX ?= g++
CFLAGS += -Iinclude -Wall -Wextra -g3

SOURCES := $(wildcard src/*.c)
OBJECTS := $(patsubst src/%.c,build/%.o,$(SOURCES))

build/main: $(OBJECTS) main.c | $(BUILD_DIR) Makefile
	$(CC) $(CFLAGS) $^ -o $@

build/%.o: src/%.c | $(BUILD_DIR) Makefile
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm -rf build *.o

build/test: GTEST_FLAGS = $(shell pkg-config --cflags --libs gtest_main)
build/test: CFLAGS += -Og -fsanitize=undefined -fsanitize=address -fsanitize=leak
build/test: $(TEST_FILES) $(OBJECTS) | $(BUILD_DIR) Makefile
	$(CXX) $(CFLAGS) $(GTEST_FLAGS) -lc -Wno-error \
		$(TEST_FILES) $(OBJECTS) -o $@

run_tests: build/test
	./build/test

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

test: run_tests

default: build/main build/test
.PHONY: clean test run_tests
