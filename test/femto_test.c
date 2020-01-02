#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "../src/file_io.h"

void test_read_file()
{
    // Given a temp file and test content
    static const char test_content[] =
       "This was a triumph!\n"
       "I'm making a note here:\n"
       "Huge success!\n";

    char *temp_filename = tmpnam(NULL);

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

    // Then its content must equal to one prepared
    assert((bytes_read == strlen(test_content)) && 
                "File sizes differ");

    assert((strcmp(content,test_content) == 0) &&
            "Content differs");
 
    putchar('.');
}

int main(int argc, char *argv[])
{
    test_read_file();

    printf("\n\e[32mALL OK\e[30m\n");
    return EXIT_SUCCESS;
}
