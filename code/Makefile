CC = arm-linux-gcc
CPPFLAGS = -Iinc -Ivendor
CFLAGS = -std=c99 -O2 -Wall -Wextra
LDLIBS = -lm
SOURCES = $(wildcard src/*.c)

.PHONY: all test clean
all: bin/test
bin/test: $(SOURCES) $(wildcard inc/*.h) vendor/stb_truetype.h
	mkdir -p bin
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) -o $@ $(LDLIBS)

test:
	mkdir -p bin
	$(CC) $(CPPFLAGS) $(CFLAGS) src/cart.c tests/cart_test.c -o bin/cart_test
	cd bin && ./cart_test

clean:
	rm -f bin/test bin/cart_test
