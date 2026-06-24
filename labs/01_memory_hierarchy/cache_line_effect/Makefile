CC ?= gcc
CFLAGS ?= -O2 -Wall -Wextra -std=c11

BUILD_DIR := build
TARGET := $(BUILD_DIR)/cache_line_effect
SRC := src/cache_line_effect.c

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SRC)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR)
