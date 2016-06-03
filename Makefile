EMACS_ROOT ?= ../..
EMACS ?= emacs

CC      = gcc
LD      = gcc
CPPFLAGS = -I$(EMACS_ROOT)/src
CFLAGS = -std=gnu99 -ggdb3 -Wall -fPIC $(CPPFLAGS)

.PHONY : test

all: getrandom.so

getrandom.so: getrandom.o
	$(LD) -shared $(LDFLAGS) -o $@ $^

getrandom.o: getrandom.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	-rm -f getrandom.so getrandom.o
