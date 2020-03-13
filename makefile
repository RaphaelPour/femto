.PHONY: all
all:
	@echo "make targets:"
	@echo "bin       Build binary"
	@echo "debug     Build debug binary with symbols"
	@echo "unittest  Build and execute unittests"
	@echo "clean     Clean up build files"

.PHONY: bin
bin:
	gcc source/*.c -Wall -o bin/femto -I include

.PHONY: debug
debug:
	gcc source/*.c -Wall -pedantic -g -o bin/femto_dbg -I include

.PHONY: unittest
unittest:
	gcc source/helper.c source/buffer.c source/terminal.c source/session.c test/*.c -g -Wall -o bin/femto_test -I include -I test
	bin/femto_test

.PHONY: clean
clean:
	rm -r bin/*
