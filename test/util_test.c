#include "femto_test.h"

void test_clamp()
{
    TEST_IT_NAME("can clamp too high number");
    int low = 0;
    int high = 1;
    int input = 2;

    int expection = 1;

    int actual = CLAMP(low,high,input);

    assert(actual == expection && "Actual run away");

    TEST_OK;
}

void test_suite_util()
{
    TEST_SUITE_NAME("Helper");
    test_clamp();
}

