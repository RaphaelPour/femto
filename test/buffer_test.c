#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <buffer.h>

#include "femto_test.h"

void test_create_buffer()
{
    TEST_IT_NAME("creates a buffer");

    Buffer *buf = fe_create_buffer();
    if( ! expect_not_null( buf )) return;

    fe_free_buffer(buf);
    
    TEST_OK
}

void test_append_to_buffer()
{
    TEST_IT_NAME("appends to a buffer");
    const char *test_string = "const char *test_string";
    Buffer *buf = fe_create_buffer();
    
    if( ! expect_not_null( buf )) return;

    fe_append_to_buffer(buf, test_string, strlen(test_string));
    
    if( ! expect_b_eq( (void*) test_string, buf->data, strlen( test_string ), buf->length  )) goto cleanup;

    TEST_OK;

cleanup:
    fe_free_buffer(buf);
}

void test_append_to_buffer_multiple_times()
{
    TEST_IT_NAME("appends to a buffer multiple times");
    char *test_strings[] = 
    {
        "A common mistake that people make",
        "when trying to design something ",
        "completely foolproof is to underestimate",
        "the ingenuity of complete fools."
    };
    
    Buffer *buf = fe_create_buffer();

    if( ! expect_not_null( buf )) return;

    for( int i=0; i < 4; i++ )
        fe_append_to_buffer(buf, test_strings[i], strlen(test_strings[i]));
    
    for( int i = 0; i < 4; i++ )
        if( ! expect_s_included(buf->data, test_strings[i] )) goto cleanup;

    TEST_OK;
    
cleanup:
    fe_free_buffer(buf);

}
void test_suite_buffer()
{
    TEST_SUITE_NAME("Buffer");

    test_create_buffer();
    test_append_to_buffer();
    test_append_to_buffer_multiple_times();
}
