BUILD_DATE=$(shell date -u)
BUILD_VERSION=$(shell git describe 2>/dev/null|| git log --pretty=format:"%h"|head -n 1)
BUILD_DEFINES=-DBUILD_DATE="\"${BUILD_DATE}\"" -DBUILD_VERSION="\"${BUILD_VERSION}\""

.PHONY: all
all:
	@echo "make targets:"
	@echo "bin       Build binary"
	@echo "debug     Build debug binary with symbols"
	@echo "unittest  Build and execute unittests"
	@echo "clean     Clean up build files"

.PHONY: bin
bin:
	gcc source/*.c -Wall -o bin/femto -I include ${BUILD_DEFINES}

.PHONY: debug
debug:
	gcc source/*.c -Wall -pedantic -g -o bin/femto_dbg -I include ${BUILD_DEFINES}

.PHONY: unittest
unittest:
	gcc source/helper.c source/buffer.c source/terminal.c source/session.c test/*.c -g -Wall -o bin/femto_test -I include -I test ${BUILD_DEFINES}
	bin/femto_test

.PHONY: clean
clean:
	rm -r bin/*
