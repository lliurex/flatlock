CC=gcc
CFLAGS=-Wall
main: flatlock.c
	$(CC) $(CFLAGS) flatlock.c -o flatlock

clean:
	rm -f flatlock
