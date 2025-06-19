BUILD ?= debug

CC = gcc

ifeq ($(BUILD), debug)
	CFLAGS = -Iinclude -Wall -Wextra -g
else ifeq ($(BUILD), release)
	CFLAGS = -Iinclude -Wall -Wextra -O2
else
    $(error Unknown build type "$(BUILD)". Use "debug" or "release".)
endif

SRC_DIR = src
INCLUDE_DIR = include
TEST_DIR = tests
BUILD_DIR = build
BIN_DIR = bin

COMMON_SRC = $(SRC_DIR)/common_utils.c $(SRC_DIR)/convert_utils.c $(SRC_DIR)/io_utils.c $(SRC_DIR)/sha256.c $(SRC_DIR)/xchacha20.c $(SRC_DIR)/verbose.c $(SRC_DIR)/opts_utils.c $(SRC_DIR)/encryptor.c $(SRC_DIR)/decryptor.c $(SRC_DIR)/file_utils.c $(SRC_DIR)/gen_utils.c
TOOL_SRC = $(SRC_DIR)/main.c

COMMON_OBJ = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(COMMON_SRC))
TOOL_OBJ = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(TOOL_SRC))

TEST_SRCS = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJS = $(patsubst $(TEST_DIR)/%.c, $(BUILD_DIR)/%.o, $(TEST_SRCS))
TEST_TARGET = $(BUILD_DIR)/test_runner

TOOL_BIN = $(BIN_DIR)/fcrypt

$(shell mkdir -p $(BUILD_DIR) $(BIN_DIR))

all: $(TOOL_BIN)

$(TOOL_BIN): $(TOOL_OBJ) $(COMMON_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(TEST_TARGET): $(TEST_OBJS) $(COMMON_OBJ)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR) output.file

debug:
	$(MAKE) BUILD=debug

release:
	$(MAKE) BUILD=release

.PHONY: all clean test
