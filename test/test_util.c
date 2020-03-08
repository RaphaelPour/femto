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

int expect_line_equal(Line expected, Line actual, char *fail_msg)
{
    if(expected.index != actual.index)
    {
        TEST_FAIL
        puts(fail_msg);
        printf("Expected index %lu, got %lu\n", expected.index, actual.index);
    }
    else if(expected.length != actual.length)
    {
        TEST_FAIL
        puts(fail_msg);
        printf("Expected length %lu, got %lu\n", expected.length, actual.length);
    }
    else if(strcmp(expected.content, actual.content) != 0)
    {
        TEST_FAIL
        puts(fail_msg);
        printf("Expected  content '%.*s', got '%.*s'\n", (int)expected.length, expected.content, (int)actual.length, actual.content);
    }
    else
    {
        return OK;
    }

    return FAIL;
}

int expect_session_equal(Session expected, Session actual, char *fail_msg)
{
    if(memcmp(&actual, &expected, sizeof(Session)) == 0) return OK;

    if(actual.filename == NULL && expected.filename != NULL)
        printf("Expected filename '%s', got null\n", expected.filename);
    else if(actual.filename != NULL && expected.filename == NULL)
        printf("Expected filename null, got '%s'\n", actual.filename);
    else if(actual.filename && expected.filename && strcmp(actual.filename,expected.filename) != 0)
        printf("Expected filename %s, got %s\n", expected.filename, actual.filename);
    else if(actual.terminal_size.width != expected.terminal_size.width)
        printf("Expected terminal width %d, got %d\n", actual.terminal_size.width, expected.terminal_size.width);
    else if(actual.terminal_size.height != expected.terminal_size.height)
        printf("Expected terminal height %d, got %d\n", actual.terminal_size.height, expected.terminal_size.height);
    else if(actual.cursor_position.x != expected.cursor_position.x)
        printf("Expected cursor position x %d, got %d\n", expected.cursor_position.x, actual.cursor_position.x);
    else if(actual.cursor_position.y != expected.cursor_position.y)
        printf("Expected cursor position y %d, got %d\n", expected.cursor_position.y, actual.cursor_position.y);
    else if(actual.offset.x != expected.offset.x)
        printf("Expected offset x %d, got %d\n", expected.offset.x, actual.offset.x);
    else if(actual.offset.y != expected.offset.y)
        printf("Expected offset y %d, got %d\n", expected.offset.y, actual.offset.y);
    else if(actual.line_count != expected.line_count)
        printf("Expected line count %lu, got %lu\n", expected.line_count, actual.line_count);
    else if(actual.content_length != expected.content_length)
        printf("Expected content length %lu, got %lu\n", expected.content_length, actual.content_length);
    else if(actual.edit_mode != expected.edit_mode)
        printf("Expected edit mode %s, got %s\n", 
                (expected.edit_mode) ? "ON" : "OFF",
                (actual.edit_mode) ? "ON" : "OFF");
    else
    {
        int i;
        for(i=0;i<expected.line_count;i++)
        {   
            if(!expect_line_equal(expected.lines[i], actual.lines[i], fail_msg))
            {
                puts(fail_msg);
                return FAIL;
            }
        }
        return OK;
    }

    puts(fail_msg);
    TEST_FAIL
    return FAIL;
}

void create_testfile(const char *filename, const char *content)
{
    FILE *temp_fd = fopen(filename, "w+");

    if(!temp_fd)
    {
        perror("Create temp file");
        exit(EXIT_FAILURE);
    }

    if(fwrite(content, 1, strlen(content), temp_fd) < strlen(content))
    {
        perror("Write temp file");
        exit(EXIT_FAILURE);
    }

    fclose(temp_fd);

}

void remove_testfile(const char *filename)
{
    unlink(filename);
}

