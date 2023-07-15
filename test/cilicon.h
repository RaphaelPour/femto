#ifndef CILICON_H
#define CILICON_H
#include <stdbool.h>

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

bool test_are_all_ok();
void test_print_result();

/* Integer */
bool expect_i_eq(int expection, int actual);
bool expect_i_not_eq(int expection, int actual);
bool expect_i_gt(int expection, int limit);
bool expect_i_gte(int expection, int limit);
bool expect_i_lt(int expection, int limit);
bool expect_i_lte(int expection, int limit);

/* Boolean  */
bool expect_true( bool actual );
bool expect_false( bool actual );

/* String */
bool expect_s_eq(char *expection, char *actual);
bool expect_s_n_eq(char *expection, char *actual, int length);
bool expect_s_not_eq(char *expection, char *actual);
bool expect_s_included(char *haystack, char *needle);

/* Pointer */
bool expect_not_null(void *actual);
bool expect_null(void *actual);

/* Buffer */
bool expect_b_eq(void *expection, void *actual, int len_expection, int len_actual);
bool expect_b_neq(void *expection, void *actual, int len_expection, int len_actual);

/* Special types */
bool expect_session_equal(Session *expected, Session *actual);
bool expect_line_equal(Line expected, Line actual);

#endif // CLICON
