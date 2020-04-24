#include "femto_test.h"

void test_session_init()
{
    TEST_IT_NAME("inits a session");

    Session *s = fe_init_session(NULL);

    if(!expect_null((void*)s->filename)) return;
    if(!expect_i_eq(1,s->cursor_position.x )) return;
    if(!expect_i_eq(1,s->cursor_position.y )) return;
    if(!expect_i_eq(0,s->offset.x )) return;
    if(!expect_i_eq(0,s->offset.y )) return;

    if(!expect_null(s->lines )) return;
    if(!expect_i_eq(0,s->content_length )) return;
    if(!expect_i_eq(0,s->line_count )) return;
    if(!expect_i_eq(0,s->edit_mode )) return;
    
    fe_free_session(s);
    TEST_OK;
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


void test_suite_session()
{
    TEST_SUITE_NAME("Session");
    test_load_file();
    test_load_file_with_long_lines();
    test_save_file();
    test_session_init();
}
