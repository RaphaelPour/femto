#include "femto_test.h"

void test_min()
{
    TEST_IT_NAME("can find the minimum");
    if(!expect_i_eq(1, MIN(1,10), "Actual 1 run away"))return;
    if(!expect_i_eq(1, MIN(10,1), "Actual 2 run away"))return;
    if(!expect_i_eq(-1, MIN(-1,10), "Actual 3 run away"))return;
    if(!expect_i_eq(-1, MIN(10,-1), "Actual 4 run away"))return;
    if(!expect_i_eq(1, MIN(1,1), "Actual 5 run away"))return;
    
    TEST_OK;
}

void test_max()
{
    TEST_IT_NAME("can find the maximum");
    if(!expect_i_eq(10, MAX(1,10), "Actual 1 run away"))return;
    if(!expect_i_eq(10, MAX(10,1), "Actual 2 run away"))return;
    if(!expect_i_eq(1, MAX(1,-10), "Actual 3 run away"))return;
    if(!expect_i_eq(1, MAX(-10,1), "Actual 4 run away"))return;
    if(!expect_i_eq(1, MAX(1,1), "Actual 5 run away"))return;

    TEST_OK;
}

void test_clamp_valid()
{
    TEST_IT_NAME("can clamp valid");

    if(!expect_i_eq(1 , CLAMP(1, 0, 2), "Actual 1 run away")) return;
    if(!expect_i_eq(1 , CLAMP(1, 1, 2), "Actual 2 run away")) return;
    if(!expect_i_eq(2 , CLAMP(2, 1, 2), "Actual 3 run away")) return;

    TEST_OK;
}

void test_clamp_too_high()
{
    TEST_IT_NAME("can clamp too high number");
    int low = 0;
    int high = 1;
    int input = 2;

    int expection = 1;

    int actual = CLAMP(low,high,input);

    if(!expect_i_eq(expection,actual, "Actual run away"))return;

    TEST_OK;
}

void test_clamp_range_inverted()
{
    TEST_IT_NAME("can clamp if range is inverted");
    int low = 10;
    int high = 0;
    int input = -1;

    int expection = 0;

    int actual = CLAMP(input, high, low);

    if(!expect_i_eq(expection,actual, "Actual run away")) return;

    actual = CLAMP(input, low, high);
    if(!expect_i_eq(expection,actual, "Actual run away")) return;

    TEST_OK;
}

void test_suite_util()
{
    TEST_SUITE_NAME("Helper");
    test_min();
    test_max();
    test_clamp_too_high();
    test_clamp_range_inverted();
    test_clamp_valid();
}

