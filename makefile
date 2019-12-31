all:
	gcc src/femto.c -Wall -pedantic -o bin/femto

debug:
	gcc src/femto.c -Wall -pedantic -g -o bin/femto_dbg

clean:
	rm -r bin/*
