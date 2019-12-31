all:
	cd src
	gcc -Wall -pedantic femto.c -o ../bin/femto
	cd ..

debug:
	cd src
	gcc -Wall -pedantic -g femto.c -o ../bin/femto_dbg
	cd ..

clean:
	rm bin/*
