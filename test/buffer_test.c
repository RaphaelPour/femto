#include "femto_test.h"

void test_create_buffer()
{
    TEST_IT_NAME("creates a buffer");

    Buffer *buf = fe_create_buffer();
    assert(buf && "Buffer is NULL");

    fe_free_buffer(buf);
    assert(!buf->data && "Buffer is not freed correctly");
    TEST_OK
}

void test_append_to_buffer()
{
    TEST_IT_NAME("appends to a buffer");
    TEST_SKIP;
    const char *test_string = "const char *test_string";
    Buffer *buf = fe_create_buffer();
    assert(buf && "Buffer is NULL");

    fe_append_to_buffer(buf, test_string, strlen(test_string));
    
    assert( strcmp(buf->data, test_string) == 0 && "Strings differ");
    fe_free_buffer(buf);
    assert(buf->data == NULL && "Buffer is not freed correctly");

    TEST_OK
}

void test_append_to_buffer_multiple_times()
{
    TEST_IT_NAME("appends to a buffer multiple times");
    TEST_SKIP;
    char *test_strings[] = 
    {
        "A common mistake that people make",
        "when trying to design something ",
        "completely foolproof is to underestimate",
        "the ingenuity of complete fools."
    };
    
    Buffer *buf = fe_create_buffer();
    assert(buf && "Buffer is NULL");

    int i;
    for(i=0;i<4;i++)
        fe_append_to_buffer(buf, test_strings[i], strlen(test_strings[i]));
    
    for(i=0;i<4;i++)
        if(expect_s_included(buf->data, test_strings[i] ) == FAIL)
        {
            TEST_FATALITY
            exit(EXIT_FAILURE);
        }

    fe_free_buffer(buf);
    assert(!buf->data && "Buffer is not freed correctly");

    TEST_OK
}
void test_suite_buffer()
{
    TEST_SUITE_NAME("Buffer");

    test_create_buffer();
    test_append_to_buffer();
    test_append_to_buffer_multiple_times();
}
