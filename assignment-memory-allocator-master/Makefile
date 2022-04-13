CFLAGS=--std=gnu11 -Wall -pedantic -Isrc/ -ggdb -Wextra -Werror -DDEBUG -g
BUILDDIR=build
SRCDIR=src
CC=gcc

all: $(BUILDDIR)/mem.o $(BUILDDIR)/util.o $(BUILDDIR)/mem_debug.o $(BUILDDIR)/main.o $(BUILDDIR)/tests/tests.o
	$(CC) -o $(BUILDDIR)/main $^

build:
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/mem.o: $(SRCDIR)/mem/mem.c build
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILDDIR)/mem_debug.o: $(SRCDIR)/mem/mem_debug.c build
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILDDIR)/util.o: $(SRCDIR)/util/util.c build
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILDDIR)/main.o: $(SRCDIR)/main.c build 
	$(CC) -c $(CFLAGS) $< -o $@

$(BUILDDIR)/tests/tests.o: $(SRCDIR)/tests/tests.c build
	mkdir -p $(BUILDDIR)/tests
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -rf $(BUILDDIR)

run: all
	./$(BUILDDIR)/main
