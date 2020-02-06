all:
	gcc src/*.c -Wall -o bin/femto -I include

debug:
	gcc src/*.c -Wall -pedantic -g -o bin/femto_dbg -I include

unittests:
	gcc src/buffer.c src/terminal.c src/file_io.c test/femto_test.c -g -Wall -o bin/femto_test -I include

clean:
	rm -r bin/*
