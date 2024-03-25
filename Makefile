CC = gcc
CFLAGS = -Wall -pedantic -std=c11
SOURCES = $(wildcard *.c)
EXECUTABLES = $(SOURCES:%.c=%)

all: $(EXECUTABLES)

%: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(EXECUTABLES)