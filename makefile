all:
	gcc source/*.c -Wall -o bin/femto -I include

debug:
	gcc source/*.c -Wall -pedantic -g -o bin/femto_dbg -I include

unittest:
	gcc source/buffer.c source/terminal.c source/session.c source/file_io.c test/femto_test.c -g -Wall -o bin/femto_test -I include

clean:
	rm -r bin/*
