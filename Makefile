CFLAGS = -Wall -g -std=c99 -Werror -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=500


all: build


build:
	gcc $(CFLAGS) main.c CheckIfDirOrFile.c ShowInodeNumber.c HandleOptionsForPrint.c LongListFormat.c traverseDirectories.c -o myls


-D_POSIX_C_SOURCE=200809L
run: build
	./myls

valgrind: build
	valgrind --leak-check=full ./myls

clean:
	rm -f myls