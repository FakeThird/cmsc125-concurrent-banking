CC      = gcc
CFLAGS  = -Wall -Wextra -pthread -O2
DFLAGS  = -g -fsanitize=thread -pthread -Wall -Wextra
LDFLAGS = -pthread

TARGET  = bankdb
SRCDIR  = src
INCDIR  = include
OBJDIR  = obj

SRCS    = $(wildcard $(SRCDIR)/*.c)
OBJS    = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

ACCOUNTS = tests/accounts.txt

# Test trace files
TRACE_SIMPLE   = tests/trace_simple.txt
TRACE_READERS  = tests/trace_readers.txt
TRACE_DEADLOCK = tests/trace_deadlock.txt
TRACE_ABORT    = tests/trace_abort.txt
TRACE_BUFFER   = tests/trace_buffer.txt

# Default deadlock strategy and tick interval
DEADLOCK  = prevention
TICK_MS   = 100

.PHONY: all debug clean test \
        test1 test2 test3_prevention test3_detection test4 test5

# -------------------------------------------------------------------
# Targets
# -------------------------------------------------------------------

all: $(OBJDIR) $(TARGET)

debug: CFLAGS = $(DFLAGS)
debug: $(OBJDIR) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) $(TARGET)

# -------------------------------------------------------------------
# Tests
# -------------------------------------------------------------------

test: test1 test2 test3_prevention test3_detection test4 test5

test1: all
	@echo "=== Test 1: No Conflicts (trace_simple.txt) ==="
	./$(TARGET) --accounts=$(ACCOUNTS) --trace=$(TRACE_SIMPLE) \
		--deadlock=$(DEADLOCK) --tick-ms=$(TICK_MS)

test2: all
	@echo "=== Test 2: Concurrent Readers (trace_readers.txt) ==="
	./$(TARGET) --accounts=$(ACCOUNTS) --trace=$(TRACE_READERS) \
		--deadlock=$(DEADLOCK) --tick-ms=$(TICK_MS)

test3_prevention: all
	@echo "=== Test 3: Deadlock - Prevention (trace_deadlock.txt) ==="
	./$(TARGET) --accounts=$(ACCOUNTS) --trace=$(TRACE_DEADLOCK) \
		--deadlock=prevention --tick-ms=$(TICK_MS) --verbose

test3_detection: all
	@echo "=== Test 3: Deadlock - Detection (trace_deadlock.txt) ==="
	./$(TARGET) --accounts=$(ACCOUNTS) --trace=$(TRACE_DEADLOCK) \
		--deadlock=detection --tick-ms=$(TICK_MS) --verbose

test4: all
	@echo "=== Test 4: Insufficient Funds (trace_abort.txt) ==="
	./$(TARGET) --accounts=$(ACCOUNTS) --trace=$(TRACE_ABORT) \
		--deadlock=$(DEADLOCK) --tick-ms=$(TICK_MS)

test5: all
	@echo "=== Test 5: Buffer Pool Saturation (trace_buffer.txt) ==="
	./$(TARGET) --accounts=$(ACCOUNTS) --trace=$(TRACE_BUFFER) \
		--deadlock=$(DEADLOCK) --tick-ms=$(TICK_MS) --verbose