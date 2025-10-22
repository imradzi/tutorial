# Makefile for clean_whitespace SQLite extension

# Compiler and flags
CC ?= cc
OS := $(shell uname -s)

# Try to get SQLite CFLAGS from pkg-config if available
SQLITE_CFLAGS := $(shell pkg-config --cflags sqlite3 2>/dev/null)

# Base compiler flags
CFLAGS ?= -O2 -fPIC -std=c11 -Wall -Wextra -Werror -fvisibility=hidden

# Add SQLite flags if available
ifneq ($(SQLITE_CFLAGS),)
    CFLAGS += $(SQLITE_CFLAGS)
endif

# Platform-specific targets
ifeq ($(OS), Linux)
    TARGET = clean_whitespace.so
    LDFLAGS = -shared
else ifeq ($(OS), Darwin)
    TARGET = clean_whitespace.dylib
    LDFLAGS = -bundle -undefined dynamic_lookup
else
    TARGET = clean_whitespace.so
    LDFLAGS = -shared
endif

# Default target
all: $(TARGET)

# Build the extension
$(TARGET): clean_whitespace.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

# Clean build artifacts
clean:
	rm -f *.so *.dylib *.dll *.o

# Test target
test: $(TARGET)
	@echo "Testing clean_whitespace extension..."
	@sqlite3 :memory: ".load ./$(TARGET)" "SELECT clean_whitespace('  hello    world  ');" || echo "Test failed"

.PHONY: all clean test
