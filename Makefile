# compiler and compiler flags
CC := gcc
CFLAGS := -g -O0 -Wall -Wextra -std=c17

# dirs relative to test/
ROOT_DIR  := ..
SRC_DIR   := .
BUILD_DIR := $(ROOT_DIR)/build/test
BIN_DIR   := $(ROOT_DIR)/bin

# library we link against
LIB_NAME := tapi
LIB_FILE := $(BIN_DIR)/lib$(LIB_NAME).so

# include paths (public headers)
CFLAGS += -I$(ROOT_DIR)/include

# link flags:
# -L../bin so -ltapi finds libtapi.so
# rpath=$$ORIGIN so test binaries (in bin/) find libtapi.so (also in bin/) at runtime
LDFLAGS := -L$(BIN_DIR) -Wl,-rpath,'$$ORIGIN'
LDLIBS  := -l$(LIB_NAME)

# each test .c becomes its own executable in bin/
TEST_SRCS := $(shell find $(SRC_DIR) -maxdepth 1 -name '*.c')
TEST_OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(TEST_SRCS))
TEST_BINS := $(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%,$(TEST_SRCS))

.PHONY: all
all: $(TEST_BINS)

$(BIN_DIR)/%: $(BUILD_DIR)/%.o $(LIB_FILE)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) $(LDLIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: run
run: all
	@set -e; for t in $(TEST_BINS); do echo "==> $$t"; $$t; done

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
	rm -f $(TEST_BINS)
