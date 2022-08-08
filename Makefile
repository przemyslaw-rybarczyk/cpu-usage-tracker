TARGET = cut
ifeq ($(CC),)
CC = clang
endif
ifeq ($(CFLAGS),)
CFLAGS = -O2 -Weverything
endif

HEADERS = $(wildcard *.h)
OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

%.o: %.c $(HEADERS)
	$(CC) -c $(CFLAGS) $< -o $@
