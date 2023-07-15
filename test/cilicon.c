#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cilicon.h"
static int totalExpects = 0;
static int failedExpects = 0; 

void test_print_result(){
    if( failedExpects == 0 )
    {
        TEST_ALL_OK;
    }
    else
    {
        printf( "%d of %d expects failed.\n", failedExpects, totalExpects );
        TEST_FATALITY
    }
}

bool test_are_all_ok(){
    return failedExpects == 0;
}

bool expect_i_eq( int expection, int actual )
{
    totalExpects++;
    if( expection == actual ) return OK;

    TEST_FAIL
    
    printf( "Expected %d, got %d\n", expection, actual );
    failedExpects++;
    return FAIL;
}


bool expect_i_not_eq( int expection, int actual )
{
    totalExpects++;
    if( expection != actual ) return OK;

    TEST_FAIL
    
    printf( "Expected not %d, got %d either way\n", expection, actual );
    failedExpects++;
    return FAIL;
}

bool expect_i_gt( int expection, int limit )
{
    totalExpects++;
    if( expection > limit ) return OK;

    TEST_FAIL;

    printf( "Expected %d greater than limit %d\n", expection, limit );
    failedExpects++;
    return FAIL;
}

bool expect_i_gte( int expection, int limit )
{
    totalExpects++;
    if( expection >= limit ) return OK;

    TEST_FAIL;

    printf( "Expected %d greater than or equal limit %d\n", expection, limit );
    failedExpects++;
    return FAIL;
}

bool expect_i_lt( int expection, int limit )
{
    totalExpects++;
    if( expection < limit ) return OK;

    TEST_FAIL;

    printf( "Expected %d lower than limit %d\n", expection, limit );
    failedExpects++;
    return FAIL;
}

bool expect_i_lte( int expection, int limit )
{
    totalExpects++;
    if( expection <= limit ) return OK;

    TEST_FAIL;

    printf( "Expected %d lower than or equal limit %d\n", expection, limit );
    failedExpects++;
    return FAIL;
}

bool expect_s_eq( char* expection, char* actual )
{
    totalExpects++;
    if( strcmp( expection,actual ) == 0 ) return OK;

    TEST_FAIL
    
    printf( "Expected '%s', got '%s'\n", expection, actual );
    failedExpects++;
    return FAIL;
}

bool expect_s_n_eq( char* expection, char* actual, int length )
{
    totalExpects++;
    if ( strncmp( expection, actual, length ) == 0 ) return OK;

    TEST_FAIL

    printf( "Expected '%*s', got '%*s'\n", length, expection, length, actual );
    failedExpects++;
    return FAIL;
}

bool expect_true( bool actual )
{
    totalExpects++;

    if( actual ) return OK;

    TEST_FAIL;

    printf("Expected true, got false");
    failedExpects++;
    return FAIL;
}

bool expect_false( bool actual )
{
    totalExpects++;

    if( ! actual ) return OK;

    TEST_FAIL;

    printf("Expected false, got true");
    failedExpects++;
    return FAIL;
}

bool expect_s_not_eq( char* expection, char* actual )
{
    totalExpects++;
    if( strcmp( expection,actual ) != 0 ) return OK;

    TEST_FAIL
    
    printf( "Expected not '%s', got '%s' either way\n", expection, actual );
    failedExpects++;
    return FAIL;
}

bool expect_s_included( char *haystack, char *needle )
{
    totalExpects++;
    if( strstr( haystack, needle )) return OK;
    
    TEST_FAIL
    
    printf( "Expected '%s' in '%s' but wasn't\n", needle, haystack );
    failedExpects++;
    return FAIL;
}

bool expect_not_null( void* actual )
{
    totalExpects++;
    if( actual ) return OK;

    TEST_FAIL
    
    puts( "Expected not null, got null" );
    failedExpects++;
    return FAIL;
}

bool expect_null( void* actual )
{
    totalExpects++;
    if( actual == NULL ) return OK;

    TEST_FAIL
    
    puts( "Expected null, got not null" );
    failedExpects++;
    return FAIL;
}


bool expect_b_eq( void *expection, void *actual, int len_expection, int len_actual ){
    
    if( len_expection != len_actual ){
        TEST_FAIL;
        printf( "Expected buffer length %d, got %d\n", len_expection, len_actual );
        failedExpects++;
        return FAIL;
    }

    if( memcmp( expection, actual, len_expection ))
    {
        TEST_FAIL;
        printf( "Expected equal buffers, got unequal.\n" );
        failedExpects++;
        return FAIL;
    }

    return OK;
}
bool expect_b_neq( void *expection, void *actual, int len_expection, int len_actual ){

    if( len_expection != len_actual ) return OK;

    if( ! memcmp( expection, actual, len_expection )) return OK;

    TEST_FAIL;
    printf( "Expected unequal buffers, got equal.\n" );
    failedExpects++;
    return FAIL;
}

bool expect_line_equal( Line expected, Line actual )
{
    totalExpects++;
    if( expected.length != actual.length )
    {
        TEST_FAIL
        printf( "Expected length %u, got %u\n", expected.length, actual.length );
    }
    else if( memcmp( expected.data, actual.data, expected.length ) != 0 )
    {
        TEST_FAIL
        printf( "Expected  data '%.*s', got '%.*s'\n", 
                ( int )expected.length, 
                expected.data, 
                ( int )actual.length, 
                actual.data );
    }
    else
    {
        return OK;
    }

    failedExpects++;
    return FAIL;
}

bool expect_session_equal( Session *expected, Session *actual )
{
    totalExpects++;
    if( memcmp( actual, expected, sizeof( Session )) == 0 ) return OK;

    if( actual->filename == NULL && expected->filename != NULL )
    {
        TEST_FAIL
        printf( "Expected filename '%s', got null\n", expected->filename );
    }
    else if( actual->filename != NULL && expected->filename == NULL )
    {
        TEST_FAIL
        printf( "Expected filename null, got '%s'\n", actual->filename );
    }
    else if( actual->filename && expected->filename && strcmp( actual->filename,expected->filename ) != 0 )
    {
        TEST_FAIL
        printf( "Expected filename %s, got %s\n", expected->filename, actual->filename );
    }
    else if( actual->terminal_size.width != expected->terminal_size.width )
    {
        TEST_FAIL
        printf( "Expected terminal width %d, got %d\n", actual->terminal_size.width, expected->terminal_size.width );
    }
    else if( actual->terminal_size.height != expected->terminal_size.height )
    {
        TEST_FAIL
        printf( "Expected terminal height %d, got %d\n", actual->terminal_size.height, expected->terminal_size.height );
    }
    else if( actual->cursor_position.x != expected->cursor_position.x )
    {
        TEST_FAIL
        printf( "Expected cursor position x %d, got %d\n", expected->cursor_position.x, actual->cursor_position.x );
    }
    else if( actual->cursor_position.y != expected->cursor_position.y )
    {
        TEST_FAIL
        printf( "Expected cursor position y %d, got %d\n", expected->cursor_position.y, actual->cursor_position.y );
    }
    else if( actual->offset.x != expected->offset.x )
    {
        TEST_FAIL
        printf( "Expected offset x %d, got %d\n", expected->offset.x, actual->offset.x );
    }
    else if( actual->offset.y != expected->offset.y )
    {
        TEST_FAIL
        printf( "Expected offset y %d, got %d\n", expected->offset.y, actual->offset.y );
    }
    else if( actual->line_count != expected->line_count )
    {
        TEST_FAIL
        printf( "Expected line count %lu, got %lu\n", expected->line_count, actual->line_count );
    }
    else if( actual->content_length != expected->content_length )
    {
        TEST_FAIL
        printf( "Expected content length %lu, got %lu\n", expected->content_length, actual->content_length );
    }
    else
    {
        int i;
        for( i=0;i<expected->line_count;i++)
        {   
            if(!expect_line_equal( expected->lines[i], actual->lines[i]))
            {
                TEST_FAIL
                failedExpects++;
                return FAIL;
            }
        }
        return OK;
    }

    
    failedExpects++;
    return FAIL;
}

void create_testfile( const char *filename, const char *data )
{
    FILE *temp_fd = fopen( filename, "w+" );

    if(!temp_fd )
    {
        perror( "Create temp file" );
        exit( EXIT_FAILURE );
    }

    if( fwrite( data, 1, strlen( data ), temp_fd ) < strlen( data ))
    {
        perror( "Write temp file" );
        exit( EXIT_FAILURE );
    }

    fclose( temp_fd );

}

void remove_testfile( const char *filename )
{
    unlink( filename );
}

