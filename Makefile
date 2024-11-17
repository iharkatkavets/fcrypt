CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -O2

SRC_DIR = src
INCLUDE_DIR = include
TEST_DIR = tests
BUILD_DIR = build
BIN_DIR = bin

COMMON_SRC = $(SRC_DIR)/common_utils.c $(SRC_DIR)/convert_utils.c $(SRC_DIR)/io_utils.c $(SRC_DIR)/sha256.c $(SRC_DIR)/xchacha20.c $(SRC_DIR)/verbose.c
ENCRYPTOR_SRC = $(SRC_DIR)/encryptor.c
DECRYPTOR_SRC = $(SRC_DIR)/decryptor.c

COMMON_OBJ = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(COMMON_SRC))
ENCRYPTOR_OBJ = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(ENCRYPTOR_SRC))
DECRYPTOR_OBJ = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(DECRYPTOR_SRC))

TEST_SRCS = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJS = $(patsubst $(TEST_DIR)/%.c, $(BUILD_DIR)/%.o, $(TEST_SRCS))
TEST_TARGET = $(BUILD_DIR)/test_runner

ENCRYPTOR_BIN = $(BIN_DIR)/senc
DECRYPTOR_BIN = $(BIN_DIR)/sdec

$(shell mkdir -p $(BUILD_DIR) $(BIN_DIR))

all: $(ENCRYPTOR_BIN) $(DECRYPTOR_BIN)

$(ENCRYPTOR_BIN): $(ENCRYPTOR_OBJ) $(COMMON_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(DECRYPTOR_BIN): $(DECRYPTOR_OBJ) $(COMMON_OBJ)
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

.PHONY: all clean test
