#include "femto_test.h"
#include "screen.h"

void test_highlight_init_c_file()
{
    TEST_IT_NAME("inits a highlighter on a file called \"my_file.c\"");

    Highlighter *h = fe_init_highlighter("my_file.c");

    if(!expect_i_not_eq( 0, h->expressions_len )) goto cleanup;
    if(!expect_not_null( h->expressions )) goto cleanup;
    if(!expect_s_eq( "c" , h->filetype )) goto cleanup;

    TEST_OK;

cleanup: 
    fe_free_highlighter(h);
}

void test_highlight_init_random_file()
{
    TEST_IT_NAME("inits a highlighter on a file called \"some_file.some_extension\"");

    Highlighter *h = fe_init_highlighter("some_file.some_extension");

    if(!expect_i_eq( 0, h->expressions_len )) goto cleanup;
    if(!expect_null( h->expressions )) goto cleanup;
    if(!expect_s_eq( "some_extension" , h->filetype )) goto cleanup;

    TEST_OK;

cleanup: 
    fe_free_highlighter(h);
}

void test_highlight_code( char *code, char *expectedColor )
{
    char *test_name = malloc( sizeof( char ) * 100 );
    (void)sprintf( test_name, "highlights \"%s\"", code );
    TEST_IT_NAME( test_name );

    Highlighter *h = fe_init_highlighter("test.c");
    Buffer *buf = fe_create_buffer();
    
    fe_append_to_buffer(buf, code, strlen(code));
    Buffer *highlighted = fe_highlight(h, buf);

    char *expected_str = malloc( sizeof( char ) * 100 );
    sprintf(expected_str, "%s%s\033[0m", expectedColor, code);

    if(!expect_s_n_eq( expected_str, highlighted->data, highlighted->length )) goto cleanup;

    TEST_OK;

cleanup:
    fe_free_highlighter(h);
    fe_free_buffer(buf);
}

void test_highlight_none( char *filename, char *code )
{
    char *test_name = malloc( sizeof( char ) * 100 );
    (void)sprintf( test_name, "doesn't highlight \"%s\" in file \"%s\"", code, filename );
    TEST_IT_NAME( test_name );

    Highlighter *h = fe_init_highlighter(filename);
    Buffer *buf = fe_create_buffer();
    
    fe_append_to_buffer(buf, code, strlen(code));
    Buffer *highlighted = fe_highlight(h, buf);

    if(!expect_s_n_eq( code, highlighted->data, strlen(code) )) goto cleanup;

    TEST_OK;

cleanup:
    fe_free_highlighter(h);
    fe_free_buffer(buf);
}

void test_suite_highlight()
{
    TEST_SUITE_NAME( "Highlight" );

    TEST_CONTEXT_NAME( "Inits" );
    test_highlight_init_c_file();
    test_highlight_init_random_file();

    TEST_CONTEXT_NAME( "Highlight Code (positive) " );
    test_highlight_code("void", RED_COLOR);
    test_highlight_code("\"I am a String\"", YELLOW_COLOR);
    test_highlight_code("42432", BLUE_COLOR);
    test_highlight_code("0b01010110", BLUE_COLOR);
    test_highlight_code("0xaf423b", BLUE_COLOR);
    test_highlight_code("0xAF423B", BLUE_COLOR);
    test_highlight_code("func(", PURPLE_COLOR);
    test_highlight_code("MyStructType", YELLOW_COLOR);

    TEST_CONTEXT_NAME( "Highlight Code (negative) " );
    test_highlight_none("test.c", "some_variable");
    test_highlight_none("test.c", ";");
    test_highlight_none("some_file.some_extension", "void");
    test_highlight_none("some_file.some_extension", "43");
    test_highlight_none("some_file.some_extension", "func(");
    test_highlight_none("some_file_without_extension", "void");
}
