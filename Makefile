CC = gcc
CFLAGS = -Wall -Wextra -fPIC -Iinclude
LDFLAGS = -shared -ldl

TARGET = libleakspy.so

SRC = src/interceptor.c \
      src/tracker.c \
      src/reporter.c

TEST_SRC = tests/no_leak.c \
           tests/memory_leak.c \
           tests/mixed_test.c \
           tests/benchmark.c

TEST_BIN = tests/no_leak \
           tests/memory_leak \
           tests/mixed_test \
           tests/benchmark

all: $(TARGET) $(TEST_BIN)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LDFLAGS) -o $(TARGET)

tests/no_leak: tests/no_leak.c
	$(CC) $< -o $@

tests/memory_leak: tests/memory_leak.c
	$(CC) $< -o $@

tests/mixed_test: tests/mixed_test.c
	$(CC) $< -o $@

tests/benchmark: tests/benchmark.c
	$(CC) $< -o $@

test: all
	@echo "===================================="
	@echo "        LeakSpy Automated Tests"
	@echo "===================================="

	@echo ""
	@echo "===== Testing NO LEAK program ====="
	@output=$$(LD_PRELOAD=./$(TARGET) ./tests/no_leak); \
	echo "$$output"; \
	if echo "$$output" | grep -q "NO MEMORY LEAK DETECTED"; then \
		echo "PASS: No leak test"; \
	else \
		echo "FAIL: No leak test"; \
		exit 1; \
	fi

	@echo ""
	@echo "===== Testing MEMORY LEAK program ====="
	@output=$$(LD_PRELOAD=./$(TARGET) ./tests/memory_leak); \
	echo "$$output"; \
	if echo "$$output" | grep -q "POSSIBLE MEMORY LEAK"; then \
		echo "PASS: Memory leak test"; \
	else \
		echo "FAIL: Memory leak test"; \
		exit 1; \
	fi

	@echo ""
	@echo "===== Testing MIXED allocation program ====="
	@output=$$(LD_PRELOAD=./$(TARGET) ./tests/mixed_test); \
	echo "$$output"; \
	if echo "$$output" | grep -q "Leaked memory     : 1000 bytes"; then \
		echo "PASS: Mixed allocation test"; \
	else \
		echo "FAIL: Mixed allocation test"; \
		exit 1; \
	fi

	@echo ""
	@echo "===================================="
	@echo "All LeakSpy tests passed: 3/3"
	@echo "===================================="

benchmark: all
	@echo "===================================="
	@echo "       LeakSpy Benchmark"
	@echo "===================================="
	@echo ""
	@echo "----- Without LeakSpy -----"
	@time ./tests/benchmark
	@echo ""
	@echo "----- With LeakSpy -----"
	@time env LD_PRELOAD=./$(TARGET) ./tests/benchmark

clean:
	rm -f $(TARGET) $(TEST_BIN)

.PHONY: all test benchmark clean
