CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -O2
LDFLAGS=-lm
OBJ=main.c front.c data.c

oh: $(OBJ) data.h front.h path.h interval.h admin.h
	$(CC) $(CFLAGS) -o oh $(OBJ) $(LDFLAGS)

run: oh
	./oh

clean:
	rm -f oh oh.exe #CC
