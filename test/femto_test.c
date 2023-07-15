#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <helper.h>

#include "femto_test.h"

#ifndef BUILD_DATE
#define BUILD_DATE "fix your makefile"
#endif

#ifndef BUILD_VERSION
#define BUILD_VERSION "fix your makefile"
#endif

int main(int argc, char *argv[])
{
    printf("BuildDate: %s\n", BUILD_DATE);
    printf("BuildVersion: %s\n", BUILD_VERSION);


    lopen("femto_test.log", LOG_DEBUG);

    test_suite_session();
    test_suite_terminal();
    test_suite_buffer();
    test_suite_util();
    test_suite_cursor();
    test_suite_highlight();

    test_print_result();

    lclose();
    return (test_are_all_ok() ? EXIT_SUCCESS : EXIT_FAILURE);
}
