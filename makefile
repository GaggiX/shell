# Usage:
# make         # compile the binary
# make install # install the binary
# make clean   # remove the binary

.PHONY = clean install

CC = gcc

shell: shell.c
	$(CC) shell.c -o shell

clean:
	rm shell

install: shell
	mv shell /bin/gsh
