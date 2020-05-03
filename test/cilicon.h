#ifndef CILICON_H
#define CILICON_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <session.h>
#define OK 1
#define FAIL 0

#define TEST_SUITE_NAME(name) printf("\e[1;34m---- %s ----\e[0m\n",name);
#define TEST_CONTEXT_NAME(name) printf("\e[0;34m---- %s ----\e[0m\n",name);
#define TEST_SKIP puts("\e[33mSKIP\e[0m");return;
#define TEST_ALL_OK puts("\n\e[32mALL PASSED\e[0m\n");
#define TEST_OK puts("\e[32mOK\e[0m");
#define TEST_FAIL puts("\e[31mFAIL\e[0m");
#define TEST_FATALITY puts("\e[31mFATALITY\e[0m");
#define TEST_IT_NAME(name) printf("it %-100s ",name);

int test_are_all_ok();
void test_print_result();

/* Integer */
int expect_i_eq(int expection, int actual);
int expect_i_not_eq(int expection, int actual);
int expect_i_gt(int expection, int limit);
int expect_i_gte(int expection, int limit);
int expect_i_lt(int expection, int limit);
int expect_i_lte(int expection, int limit);

/* String */
int expect_s_eq(char *expection, char *actual);
int expect_s_not_eq(char *expection, char *actual);
int expect_s_included(char *haystack, char *needle);

/* Pointer */
int expect_not_null(void *actual);
int expect_null(void *actual);

/* Buffer */
int expect_b_eq(void *expection, void *actual, int len_expection, int len_actual);
int expect_b_neq(void *expection, void *actual, int len_expection, int len_actual);

/* Special types */
int expect_session_equal(Session *expected, Session *actual);
int expect_line_equal(Line expected, Line actual);

#endif // CLICON
