BUILD_DIR ?= build

AR ?= ar
CC  ?= gcc
CXX ?= g++
OBJDUMP ?= objdump

CFLAGS += -Iinclude -Wall -Wextra -g3
CXXFLAGS += -std=c++23
SANITIZE_FLAGS = -fsanitize=address -fsanitize=undefined -fsanitize=leak
COV_FLAGS = --coverage -ftest-coverage -fprofile-abs-path
DEPFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

SOURCES := $(wildcard src/*.c)
OBJECTS := $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(SOURCES))
TEST_FILES = $(wildcard test/*.cpp)

ifdef SANITIZE
CFLAGS += $(SANITIZE_FLAGS)
endif

ifdef OPTIM
CFLAGS += -O3
endif

$(BUILD_DIR)/main: $(OBJECTS) main.c | $(BUILD_DIR) Makefile
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: src/%.c | $(BUILD_DIR) Makefile
	$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

$(BUILD_DIR)/libnetbuf.a: $(OBJECTS) | $(BUILD_DIR) Makefile
	$(AR) rcs $@ $^

%.lst: %.o
	$(OBJDUMP) $< -h -d -S > $@ &

%.lst: %.elf
	$(OBJDUMP) $< -h -d -S > $@ &

disassemble: $(OBJECTS:.o=.lst)

clean:
	rm -rf $(BUILD_DIR) *.o

TEST_RUNNERS = $(patsubst test/%.cpp,$(BUILD_DIR)/test_%.o,$(TEST_FILES))
GTEST_FLAGS := $(shell pkg-config --cflags --libs gtest_main 2>/dev/null)
GTEST_FLAGS += $(CFLAGS) -Og $(SANITIZE_FLAGS)

$(BUILD_DIR)/test_%.o: test/%.cpp | $(BUILD_DIR) Makefile
	$(CXX) $(CXXFLAGS) $(GTEST_FLAGS) $(DEPFLAGS) -c $< -o $@

$(BUILD_DIR)/test: CFLAGS += $(SANITIZE_FLAGS) $(COV_FLAGS)
$(BUILD_DIR)/test: $(TEST_RUNNERS) $(OBJECTS) | $(BUILD_DIR) Makefile
	$(CXX) $(CXXFLAGS) $(GTEST_FLAGS) $(COV_FLAGS) $^ -o $@

test: $(BUILD_DIR)/test
	$(BUILD_DIR)/test
	@gcovr --lcov report.info 2> /dev/null

coverage:
	@gcovr 2>/dev/null
	@gcovr --html-details -o $(BUILD_DIR)/coverage.html 2> /dev/null

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

lib: $(BUILD_DIR)/libnetbuf.a

default: $(BUILD_DIR)/main lib

all: default disassemble test

-include $(shell find -name "*.d" -type f)

.DEFAULT_GOAL := default
.PHONY: clean test
