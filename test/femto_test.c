#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "femto_test.h"

int main(int argc, char *argv[])
{
    test_suite_session();
    test_suite_terminal();
    test_suite_buffer();
    test_suite_util();
    test_suite_cursor();

    test_print_result();

    return (test_are_all_ok() ? EXIT_SUCCESS : EXIT_FAILURE);
}
