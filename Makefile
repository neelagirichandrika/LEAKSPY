CC = gcc
CFLAGS = -Wall -Wextra -fPIC -Iinclude
LDFLAGS = -shared -ldl

TARGET = libleakspy.so

SRC = src/interceptor.c \
      src/tracker.c \
      src/reporter.c

TEST_SRC = tests/no_leak.c tests/memory_leak.c

TEST_BIN = tests/no_leak tests/memory_leak

all: $(TARGET) $(TEST_BIN)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LDFLAGS) -o $(TARGET)

tests/no_leak: tests/no_leak.c
	$(CC) $< -o $@

tests/memory_leak: tests/memory_leak.c
	$(CC) $< -o $@

test: all
	@echo "===== Testing NO LEAK program ====="
	LD_PRELOAD=./$(TARGET) ./tests/no_leak
	@echo ""
	@echo "===== Testing MEMORY LEAK program ====="
	LD_PRELOAD=./$(TARGET) ./tests/memory_leak

clean:
	rm -f $(TARGET) $(TEST_BIN)

.PHONY: all test clean
