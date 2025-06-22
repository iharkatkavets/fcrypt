BUILD ?= debug
LUAJIT_INC ?= /opt/homebrew/include/luajit-2.1
LUAJIT_LIB ?= /opt/homebrew/lib

CC = gcc

ifeq ($(BUILD), debug)
	CFLAGS = -Iinclude -Wall -Wextra -g -DDEBUG
else ifeq ($(BUILD), release)
	CFLAGS = -Iinclude -Wall -Wextra -O2
else
    $(error Unknown build type "$(BUILD)". Use "debug" or "release".)
endif

SRC_DIR = src
TEST_DIR := test
INCLUDE_DIR = include
BUILD_DIR = build
BIN_DIR = bin

COMMON_SRC = $(SRC_DIR)/common_utils.c $(SRC_DIR)/convert_utils.c $(SRC_DIR)/io_utils.c $(SRC_DIR)/sha256.c $(SRC_DIR)/xchacha20.c $(SRC_DIR)/verbose.c $(SRC_DIR)/opts_utils.c $(SRC_DIR)/encrypt.c $(SRC_DIR)/decrypt.c $(SRC_DIR)/file_utils.c $(SRC_DIR)/gen_utils.c
TOOL_SRC = $(SRC_DIR)/main.c
LUA_SRC = $(SRC_DIR)/lua_binding.c

TEST_SRCS := $(wildcard $(TEST_DIR)/test_*.c)
TEST_BINS := $(TEST_SRCS:$(TEST_DIR)/%.c=$(BUILD_DIR)/%)

COMMON_OBJ = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(COMMON_SRC))
TOOL_OBJ = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(TOOL_SRC))

TOOL = fcrypt
TOOL_PATH = $(BIN_DIR)/$(TOOL)
LUA_MODULE = fcrypt_lua_mod.so
LUA_MODULE_PATH = $(BIN_DIR)/$(LUA_MODULE)

$(shell mkdir -p $(BUILD_DIR) $(BIN_DIR))

all: $(TOOL_PATH) $(LUA_MODULE_PATH)

$(TOOL_PATH): $(TOOL_OBJ) $(COMMON_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

lua: $(LUA_MODULE_PATH)

$(LUA_MODULE_PATH): $(LUA_SRC) $(COMMON_SRC)
	$(CC) $(CFLAGS) -fPIC -shared -I$(LUAJIT_INC) -L$(LUAJIT_LIB) -lluajit-5.1 -o $@ $^

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR) output.file

debug:
	$(MAKE) BUILD=debug

release:
	$(MAKE) BUILD=release

$(BUILD_DIR)/test_%: $(TEST_DIR)/test_%.c $(COMMON_OBJ)
	$(CC) $(CFLAGS) $^ -o $@

test: $(TEST_BINS)
	@for bin in $(TEST_BINS); do echo "Running $$bin..."; $$bin || exit 1; done

.PHONY: all clean lua test
