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
    TEST_ALL_OK
    return EXIT_SUCCESS;
}
