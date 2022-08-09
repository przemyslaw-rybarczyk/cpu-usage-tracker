TARGET = cut
ifeq ($(CC),)
CC = clang
endif
ifeq ($(CFLAGS),)
CFLAGS = -O2 -Weverything -Wno-atomic-implicit-seq-cst
endif
TEST_TARGET = cut-test

HEADERS = $(wildcard *.h)
OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

.PHONY: test

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(filter-out test.o,$(OBJECTS)) -o $@

%.o: %.c $(HEADERS)
	$(CC) -c $(CFLAGS) $< -o $@

$(TEST_TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(filter-out main.o,$(OBJECTS)) -o $(TEST_TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)
