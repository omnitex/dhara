# Simple Makefile for testing bch.c

CC = gcc
CFLAGS = -Wall -Wextra -g

# Source files
SRCS = main.c bch.c gf13.c
OBJS = $(SRCS:.c=.o)

# Output binary
TARGET = bch_test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
run: all
	./bch_test
