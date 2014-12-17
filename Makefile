cc = gcc
src = src/*
out = cc8

cflags = -ansi -pedantic -Wall -Wextra -O2 -mtune=native -pipe
ldflags = -lSDL2

dest=/

all: $(src)
	$(cc) $(ldflags) $(cflags) $(src) -o $(out)
install:
	install -m 755 $(out) $(dest)/usr/bin/
uninstall:
	rm -f $(dest)/usr/bin/$(out)
clean:
	rm -f $(out)
