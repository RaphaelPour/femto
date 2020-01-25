all:
	gcc src/*.c -Wall -pedantic -o bin/femto

debug:
	gcc src/*.c -Wall -pedantic -g -o bin/femto_dbg

unittests:
	gcc src/buffer.c src/terminal.c src/file_io.c test/femto_test.c -g -Wall -o bin/femto_test

clean:
	rm -r bin/*
