#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "../src/file_io.h"
#include "../src/terminal.h"

#define TEST_SUITE_NAME(name) printf("---- %s\n",name);
#define TEST_SKIP puts("\e[33m   SKIP\e[0m");return;
#define TEST_ALL_OK puts("\n\e[32mALL OK\e[0m\n");
#define TEST_OK puts("\e[32m   OK\e[0m");
#define TEST_FAIL puts("\e31m    FAIL\e[0m");
#define TEST_IT_NAME(name) printf("it %s ",name);

void test_load_file()
{
    TEST_IT_NAME("loads a file");
    // Given a temp file and test content
    char test_content[] =
       "This was a triumph!\n"
       "I'm making a note here:\n"
       "Huge success!\n";

    char *temp_filename = "/tmp/femto.testdata";

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

    // When we load this file
    char *content = NULL;
    size_t bytes_read = fe_file_load(temp_filename, &content);

    unlink(temp_filename);

    // Then its content must equal to one prepared
    assert((bytes_read == strlen(test_content)) && 
            "Content sizes differ");

    assert((strcmp(content,test_content) == 0) &&
            "Content differs");
    
    TEST_OK
}

void test_save_file()
{
    TEST_IT_NAME("saves a file");

    char *test_content = 
        "We do what we must\n"
        "because we can.";

    char *temp_filename = "/tmp/femto.testdata2";

    fe_file_save(temp_filename, test_content, strlen(test_content));

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

    assert((bytes_read == strlen(test_content)) &&
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

    terminal_size s = fe_terminal_size();

    assert(s.rows>0 && "Rows are empty");
    assert(s.cols>0 && "Cols are empty");

    TEST_OK
    
    printf("\tNOTE: %d rows and %d cols\n", s.rows, s.cols);
}
void test_suite_terminal()
{
    TEST_SUITE_NAME("Terminal");

    test_enable_and_disable_raw_mode_without_error();
    test_get_terminal_size_without_error();
}

int main(int argc, char *argv[])
{
    test_suite_file_io();
    test_suite_terminal();
    TEST_ALL_OK
    return EXIT_SUCCESS;
}
