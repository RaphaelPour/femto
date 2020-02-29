#include "test_util.h"

int expect_i_eq(int expection, int actual, char *fail_msg)
{
    if(expection == actual) return OK;

    TEST_FAIL
    puts(fail_msg);
    printf("expected %d, got %d\n", expection, actual);
    return FAIL;
}


int expect_i_not_eq(int expection, int actual, char *fail_msg)
{
    if(expection != actual) return OK;

    TEST_FAIL
    puts(fail_msg);
    printf("expected not %d, got %d either way\n", expection, actual);
    return FAIL;
}

int expect_s_eq(char* expection, char* actual, char *fail_msg)
{
    if(strcmp(expection,actual) == 0) return OK;

    TEST_FAIL
    puts(fail_msg);
    printf("expected '%s', got '%s'\n", expection, actual);
    return FAIL;
}


int expect_s_not_eq(char* expection, char* actual, char *fail_msg)
{
    if(strcmp(expection,actual) != 0) return OK;

    TEST_FAIL
    puts(fail_msg);
    printf("expected not '%s', got '%s' either way\n", expection, actual);
    return FAIL;
}

int expect_s_included(char *haystack, char *needle, char *fail_msg)
{
    if(strstr(haystack, needle)) return OK;
    
    TEST_FAIL
    puts(fail_msg);
    printf("expected '%s' in '%s' but wasn't\n", needle, haystack);
    return FAIL;
}

int expect_not_null(void* actual, char *fail_msg)
{
    if(actual) return OK;

    TEST_FAIL
    puts(fail_msg);
    puts("expected not null, got null");
    return FAIL;
}

int expect_null(void* actual, char *fail_msg)
{
    if(actual == NULL) return OK;

    TEST_FAIL
    puts(fail_msg);
    puts("expected null, got not null");
    return FAIL;
}
