#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <file_io.h>
#include <terminal.h>
#include <buffer.h>
#include <session.h>

#define OK 1
#define FAIL 0

#define TEST_SUITE_NAME(name) printf("---- %s\n",name);
#define TEST_SKIP puts("\e[33m   SKIP\e[0m");return;
#define TEST_ALL_OK puts("\n\e[32mALL OK\e[0m\n");
#define TEST_OK puts("\e[32m   OK\e[0m");
#define TEST_FAIL puts("\e[31m    FAIL\e[0m");
#define TEST_FATALITY puts("\e[31m FATALITY\e[0m");
#define TEST_IT_NAME(name) printf("it %s ",name);

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

void test_load_file()
{
    TEST_IT_NAME("loads a file");
    // Given a temp file and test content
    char test_content[] =
       "This was a triumph!\n"
       "I'm making a note here:\n"
       "Huge success!\n";

    char *temp_filename = "/tmp/femto.testdata";
    size_t line_count = 3;

    FILE *temp_fd = fopen(temp_filename, "w+");

    if(!temp_fd)
    {
        perror("Create temp file");
        exit(EXIT_FAILURE);
    }

    if(fwrite(test_content, 1, strlen(test_content), temp_fd) < strlen(test_content))
    {
        perror("Write temp file");
        exit(EXIT_FAILURE);
    }

    fclose(temp_fd);

    Session *s = fe_init_session(temp_filename);

    unlink(temp_filename);

    // Then its content must equal to one prepared
    
    // Subtract number of new lines which equals the line count
    assert((s->content_length == strlen(test_content) - line_count) &&
            "Content sizes differ");

    assert((s->line_count == line_count) && "Lines missing");
    
    TEST_OK
}

void test_load_file_with_long_lines()
{
    TEST_IT_NAME("loads a file with long lines");

    // Given a temp file and test content
    char test_content[1026] = {'\0'};
    
    int i;
    for(i=0;i<1025;i++)
        test_content[i] = 'x';
    
        char *temp_filename = "/tmp/femto.testdata";
    size_t line_count = 1;

    FILE *temp_fd = fopen(temp_filename, "w+");

    if(!temp_fd)
    {
        perror("Create temp file");
        exit(EXIT_FAILURE);
    }

    if(fwrite(test_content, 1, strlen(test_content), temp_fd) < strlen(test_content))
    {
        perror("Write temp file");
        exit(EXIT_FAILURE);
    }

    fclose(temp_fd);

    Session *s = fe_init_session(temp_filename);

    unlink(temp_filename);

    // Then its content must equal to one prepared
    assert((s->content_length == strlen(test_content)) &&
            "Content sizes differ");

    // And we get one line more since the file read algorithm will
    // create a new line when the line length exeeds the buffer size
    assert((s->line_count == line_count+1) && "Lines missing");
    
    TEST_OK
}


void test_save_file()
{
    TEST_IT_NAME("saves a file");

    char *test_content = 
        "We do what we must\n"
        "because we can.";

    char *temp_filename = "/tmp/femto.testdata2";

    Buffer *b = fe_create_buffer();
    b->data = test_content;
    b->length = strlen(test_content);

    fe_file_save(temp_filename,b);

    FILE *temp_fd = fopen(temp_filename, "r");

    if(!temp_fd)
    {
        perror("Open temp file");
        exit(EXIT_FAILURE);
    }

    char buffer[64];

    size_t bytes_read = fread(&buffer, 1, sizeof(buffer), temp_fd);

    fclose(temp_fd);
    unlink(temp_filename);

    assert((bytes_read == b->length) &&
            "Content size differs");

    // Terminate string properly to avoid comparing to memory garbage
    buffer[bytes_read] = 0;

    assert((strcmp(buffer,test_content)==0) &&
            "Content differs");

    TEST_OK
}

void test_suite_file_io()
{
    TEST_SUITE_NAME("FileIO");
    test_load_file();
    test_load_file_with_long_lines();
    test_save_file();
}

void test_enable_and_disable_raw_mode_without_error()
{
    TEST_IT_NAME("enables and disables raw mode without an error");

    fe_enable_raw_mode();
    fe_disable_raw_mode();

    TEST_OK
}

void test_get_terminal_size_without_error()
{
    TEST_IT_NAME("gets terminal size without an error");

    TerminalSize s = fe_terminal_size();

    assert(s.rows>0 && "Rows are empty");
    assert(s.cols>0 && "Cols are empty");

    TEST_OK
}

void test_get_cursor_position_without_error()
{
    TEST_IT_NAME("gets the cursor position without an error");

    fe_enable_raw_mode();
    TerminalPosition p = fe_get_cursor_position();
    fe_disable_raw_mode();

    assert(p.x == 0 && "X position is wrong");
    assert(p.y == 0 && "Y position is wrong");

    TEST_OK
}

void test_set_and_get_cursor_position_without_error()
{
    TEST_IT_NAME("sets and gets a random cursor position without an error");

    fe_enable_raw_mode();
    
    // Set random cursor position
    TerminalPosition p = fe_get_cursor_position();

    // Get current cursor position
    fe_disable_raw_mode();

    // Check if the cursor posistion is right
    assert(p.x == 0 && "X position is wrong");
    assert(p.y == 0 && "Y position is wrong");

    TEST_OK
}

void test_suite_terminal()
{
    TEST_SUITE_NAME("Terminal");

    test_enable_and_disable_raw_mode_without_error();
    test_get_terminal_size_without_error();
    test_get_cursor_position_without_error();
}

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
        if(expect_s_included(buf->data, test_strings[i], "String missing in buffer") == FAIL)
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

int main(int argc, char *argv[])
{
    test_suite_file_io();
    test_suite_terminal();
    test_suite_buffer();
    TEST_ALL_OK
    return EXIT_SUCCESS;
}
