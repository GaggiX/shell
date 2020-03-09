# Usage:
# make         # compile the binary
# make install # install the binary
# make clean   # remove the binary

.PHONY = clean install

CC = gcc
CFLAGS  = -Wall

shell: shell.c
	$(CC) $(CFLAGS) -o shell shell.c

clean:
	rm shell

install: shell
	mv shell /bin/gsh
