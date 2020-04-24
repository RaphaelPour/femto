#include "cilicon.h"

static int totalExpects = 0;
static int failedExpects = 0; 

void test_print_result(){
    if(failedExpects == 0)
    {
        TEST_ALL_OK;
    }
    else
    {
        printf("%d of %d expects failed.\n", failedExpects, totalExpects);
        TEST_FATALITY
    }
}

int test_are_all_ok(){
    return failedExpects == 0;
}

int expect_i_eq(int expection, int actual)
{
    totalExpects++;
    if(expection == actual) return OK;

    TEST_FAIL
    
    printf("Expected%d, got %d\n", expection, actual);
    failedExpects++;
    return FAIL;
}


int expect_i_not_eq(int expection, int actual)
{
    totalExpects++;
    if(expection != actual) return OK;

    TEST_FAIL
    
    printf("Expectednot %d, got %d either way\n", expection, actual);
    failedExpects++;
    return FAIL;
}

int expect_s_eq(char* expection, char* actual)
{
    totalExpects++;
    if(strcmp(expection,actual) == 0) return OK;

    TEST_FAIL
    
    printf("Expected'%s', got '%s'\n", expection, actual);
    failedExpects++;
    return FAIL;
}


int expect_s_not_eq(char* expection, char* actual)
{
    totalExpects++;
    if(strcmp(expection,actual) != 0) return OK;

    TEST_FAIL
    
    printf("Expectednot '%s', got '%s' either way\n", expection, actual);
    failedExpects++;
    return FAIL;
}

int expect_s_included(char *haystack, char *needle)
{
    totalExpects++;
    if(strstr(haystack, needle)) return OK;
    
    TEST_FAIL
    
    printf("Expected'%s' in '%s' but wasn't\n", needle, haystack);
    failedExpects++;
    return FAIL;
}

int expect_not_null(void* actual)
{
    totalExpects++;
    if(actual) return OK;

    TEST_FAIL
    
    puts("Expectednot null, got null");
    failedExpects++;
    return FAIL;
}

int expect_null(void* actual)
{
    totalExpects++;
    if(actual == NULL) return OK;

    TEST_FAIL
    
    puts("Expectednull, got not null");
    failedExpects++;
    return FAIL;
}


int expect_b_eq(void *expection, void *actual, int len_expection, int len_actual){
    
    if( len_expection != len_actual ){
        TEST_FAIL;
        printf("Expectedbuffer length %d, got %d\n", len_expection, len_actual);
        failedExpects++;
        return FAIL;
    }

    if( ! memcmp( expection, actual, len_expection) )
    {
        TEST_FAIL;
        printf("Expectedbuffer is unequal to the actual.\n");
        failedExpects++;
        return FAIL;
    }

    return OK;
}
int expect_b_neq(void *expection, void *actual, int len_expection, int len_actual){

    if( len_expection != len_actual ) return OK;

    if( ! memcmp( expection, actual, len_expection ) ) return OK;

    TEST_FAIL;
    printf("Expectedbuffer is equal to the actual.\n");
    failedExpects++;
    return FAIL;
}

int expect_line_equal(Line expected, Line actual)
{
    totalExpects++;
    if(expected.index != actual.index)
    {
        TEST_FAIL
        printf("Expectedindex %lu, got %lu\n", expected.index, actual.index);
    }
    else if(expected.length != actual.length)
    {
        TEST_FAIL
        printf("Expectedlength %lu, got %lu\n", expected.length, actual.length);
    }
    else if(strcmp(expected.content, actual.content) != 0)
    {
        TEST_FAIL
        printf("Expected content '%.*s', got '%.*s'\n", 
                (int)expected.length, 
                expected.content, 
                (int)actual.length, 
                actual.content);
    }
    else
    {
        return OK;
    }

    failedExpects++;
    return FAIL;
}

int expect_session_equal(Session *expected, Session *actual)
{
    totalExpects++;
    if(memcmp(actual, expected, sizeof(Session)) == 0) return OK;

    if(actual->filename == NULL && expected->filename != NULL)
    {
        TEST_FAIL
        printf("Expectedfilename '%s', got null\n", expected->filename);
    }
    else if(actual->filename != NULL && expected->filename == NULL)
    {
        TEST_FAIL
        printf("Expectedfilename null, got '%s'\n", actual->filename);
    }
    else if(actual->filename && expected->filename && strcmp(actual->filename,expected->filename) != 0)
    {
        TEST_FAIL
        printf("Expectedfilename %s, got %s\n", expected->filename, actual->filename);
    }
    else if(actual->terminal_size.width != expected->terminal_size.width)
    {
        TEST_FAIL
        printf("Expectedterminal width %d, got %d\n", actual->terminal_size.width, expected->terminal_size.width);
    }
    else if(actual->terminal_size.height != expected->terminal_size.height)
    {
        TEST_FAIL
        printf("Expectedterminal height %d, got %d\n", actual->terminal_size.height, expected->terminal_size.height);
    }
    else if(actual->cursor_position.x != expected->cursor_position.x)
    {
        TEST_FAIL
        printf("Expectedcursor position x %d, got %d\n", expected->cursor_position.x, actual->cursor_position.x);
    }
    else if(actual->cursor_position.y != expected->cursor_position.y)
    {
        TEST_FAIL
        printf("Expectedcursor position y %d, got %d\n", expected->cursor_position.y, actual->cursor_position.y);
    }
    else if(actual->offset.x != expected->offset.x)
    {
        TEST_FAIL
        printf("Expectedoffset x %d, got %d\n", expected->offset.x, actual->offset.x);
    }
    else if(actual->offset.y != expected->offset.y)
    {
        TEST_FAIL
        printf("Expectedoffset y %d, got %d\n", expected->offset.y, actual->offset.y);
    }
    else if(actual->line_count != expected->line_count)
    {
        TEST_FAIL
        printf("Expectedline count %lu, got %lu\n", expected->line_count, actual->line_count);
    }
    else if(actual->content_length != expected->content_length)
    {
        TEST_FAIL
        printf("Expectedcontent length %lu, got %lu\n", expected->content_length, actual->content_length);
    }
    else if(actual->edit_mode != expected->edit_mode)
    {
        TEST_FAIL
        printf("Expectededit mode %s, got %s\n", 
                (expected->edit_mode) ? "ON" : "OFF",
                (actual->edit_mode) ? "ON" : "OFF");
    }
    else
    {
        int i;
        for(i=0;i<expected->line_count;i++)
        {   
            if(!expect_line_equal(expected->lines[i], actual->lines[i]))
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

