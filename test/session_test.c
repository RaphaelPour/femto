#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <session.h>
#include <fileio.h>
#include <helper.h>

#include "femto_test.h"

void test_session_init()
{
    TEST_IT_NAME("inits an empty session");

    Session *s = fe_init_session(NULL);

    if(!expect_null((void*)s->filename)) return;
    if(!expect_i_eq(1,s->cursor_position.x )) return;
    if(!expect_i_eq(1,s->cursor_position.y )) return;
    if(!expect_i_eq(0,s->offset.x )) return;
    if(!expect_i_eq(0,s->offset.y )) return;

    if(!expect_not_null(s->lines )) return;
    if(!expect_i_eq(0,s->content_length )) return;
    if(!expect_i_eq(1,s->line_count )) return;
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
    if( ! expect_i_eq( s->content_length, strlen(test_content) - line_count )) return;

    if( ! expect_i_eq( s->line_count, line_count )) return;
    
    fe_free_session( s );

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
    if( ! expect_i_eq( s->content_length, strlen( test_content ))) return;

    // And we get one line more since the file read algorithm will
    // create a new line when the line length exeeds the buffer size
    if( ! expect_i_eq( s->line_count, line_count + 1 )) return;
    
    fe_free_session( s );

    TEST_OK
}

void test_load_empty_file()
{
    TEST_IT_NAME("loads an empty file");

    char *temp_filename = "/tmp/femto.testdata";
    FILE *temp_fd = fopen(temp_filename, "w+");
    
    if(!temp_fd)
    {
        perror("Create temp file");
        exit(EXIT_FAILURE);
    }

    fclose(temp_fd);

    Session *s = fe_init_session(temp_filename);

    unlink(temp_filename);

    if( ! expect_i_eq( 0, s->content_length )) goto cleanup;
    
    /* 
     * There should be one line which will be added automatically
     * when a loaded file is empty. This is needed by the editor
     * to work with a session.
     */
    if( ! expect_i_eq( 1, s->line_count )) goto cleanup;
    

    TEST_OK;

cleanup:
    fe_free_session( s );
}


void test_save_new_file()
{
    TEST_IT_NAME("saves a new file");


    /* Create a 'new file' session and insert one char */
    char test_content = '#';
    Session *s = fe_init_session( NULL );
    fe_insert_char( s, test_content );

    /* Save content ot a file */
    char *temp_filename = "/tmp/femto.testdata";
    fe_set_filename( s, temp_filename );
    fe_file_save( s );
    fe_free_session( s );

    FILE *temp_fd = fopen(temp_filename, "r");

    if( ! temp_fd )
    {
        lprintf( LOG_ERROR, "Error open file %s", temp_filename );
        return;
    }

    char buffer[8];

    size_t bytes_read = fread(&buffer, 1, sizeof(buffer), temp_fd);
    fclose(temp_fd);
    unlink(temp_filename);

    if( ! expect_i_eq( 2, bytes_read )) return;

    /* Expect test content and new line */
    if( ! expect_i_eq( test_content, buffer[0] )) return;
    if( ! expect_i_eq( '\n', buffer[1] )) return;

    TEST_OK;
}

void test_save_empty_new_file()
{
    TEST_IT_NAME( "saves an empty new file" );

    /* Create a 'new file' session and insert one char */
    Session *s = fe_init_session( NULL );
    /* Save content ot a file */
    char *temp_filename = "/tmp/femto.testdata";
    fe_set_filename( s, temp_filename );
    fe_file_save( s );
    fe_free_session( s );

    FILE *temp_fd = fopen(temp_filename, "r");

    if( ! temp_fd )
    {
        lprintf( LOG_ERROR, "Error open file %s", temp_filename );
        return;
    }

    char buffer[8];

    size_t bytes_read = fread(&buffer, 1, sizeof(buffer), temp_fd);
    fclose(temp_fd);
    unlink(temp_filename);
    
    if( ! expect_i_eq( 0, bytes_read )) return;
    
    TEST_OK;
}

void test_save_existing_file_without_change()
{
    TEST_IT_NAME( "saves an existing file without change" );

    /* Create a new empty file */
    char *temp_filename = "/tmp/femto.testdata";
    char *test_content = "#!/bin/bash\n\necho This was triumph!\n";

    FILE *temp_fd = fopen( temp_filename, "w" );
    if( ! temp_fd )
    {
        perror("Opening temp file");
        return;
    }
    
    if(fwrite(test_content, 1, strlen(test_content), temp_fd) < strlen(test_content))
    {
        perror("Write temp file");
        return;
    }

    fclose( temp_fd );

    /* Read in new file and save it immediately */
    Session *s = fe_init_session( temp_filename );
    fe_file_save( s );
    fe_free_session( s );

    temp_fd = fopen(temp_filename, "r");

    if( ! temp_fd )
    {
        lprintf( LOG_ERROR, "Error open file %s", temp_filename );
        return;
    }

    char buffer[64];

    size_t bytes_read = fread(&buffer, 1, sizeof(buffer), temp_fd);
    fclose(temp_fd);
    unlink(temp_filename);
    
    if( ! expect_i_eq( strlen( test_content ) , bytes_read )) return;
    if( ! expect_b_eq( test_content, buffer, strlen( test_content ), bytes_read )) return;
    TEST_OK;
    
}

void test_save_empty_existing_file_without_change()
{
    TEST_IT_NAME( "saves an existing empty file" );
    
    /* Create a new empty file */
    char *temp_filename = "/tmp/femto.testdata";

    FILE *temp_fd = fopen( temp_filename, "w" );
    if( ! temp_fd )
    {
        lprintf( LOG_ERROR, "Error creating test file %s", temp_filename );
        return;
    }

    fclose( temp_fd );

    /* Read in new file and save it immediately */
    Session *s = fe_init_session( temp_filename );
    fe_file_save( s );
    fe_free_session( s );

    temp_fd = fopen(temp_filename, "r");

    if( ! temp_fd )
    {
        lprintf( LOG_ERROR, "Error open file %s", temp_filename );
        return;
    }

    char buffer[8];

    size_t bytes_read = fread(&buffer, 1, sizeof(buffer), temp_fd);
    fclose(temp_fd);
    unlink(temp_filename);
    
    if( ! expect_i_eq( 0, bytes_read )) return;

    TEST_OK;
}

void test_truncates_existing_file()
{
    TEST_IT_NAME("truncates an existing file");

    /* Create a new empty file */
    char *temp_filename = "/tmp/femto.testdata";
    char *test_content = "/* true clone */ int main(){return 0;}";

    FILE *temp_fd = fopen( temp_filename, "w" );
    if( ! temp_fd )
    {
        perror("Opening temp file");
        return;
    }
    
    if(fwrite(test_content, 1, strlen(test_content), temp_fd) < strlen(test_content))
    {
        perror("Write temp file");
        return;
    }

    fclose( temp_fd );

    /* Read in new file, truncate it and save it immediately */
    Session *s = fe_init_session( temp_filename );

    /* 
     * Truncate file by setting cursor one step to the right and simulate
     * backspace for every char of the test string
     */
    for( int i = 0; i < strlen( test_content ); i++ )
    {
        fe_move( s, 1, 0 );
        fe_remove_char_at_cursor( s );
    }

    fe_file_save( s );
    fe_free_session( s );

    temp_fd = fopen(temp_filename, "r");

    if( ! temp_fd )
    {
        lprintf( LOG_ERROR, "Error open file %s", temp_filename );
        return;
    }

    char buffer[8];

    size_t bytes_read = fread(&buffer, 1, sizeof(buffer), temp_fd);
    fclose(temp_fd);
    unlink(temp_filename);
    
    if( ! expect_i_eq( 0 , bytes_read )) return;
    TEST_OK;

}


void test_invalid_load_file_without_filename()
{
    TEST_IT_NAME( "doesn't load a file without filename" );

    Session *s = fe_init_session( NULL );
    
    if( ! expect_false( fe_file_load( s ))) goto cleanup;

    TEST_OK;

cleanup:
    fe_free_session( s );
}

void test_invalid_save_file_without_filename()
{
    TEST_IT_NAME( "doesn't save a file without filename" );

    Session *s = fe_init_session( NULL );
    
    if( ! expect_false( fe_file_save( s ))) goto cleanup;

    TEST_OK;

cleanup:
    fe_free_session( s );
}


void test_fixup_issue_26(){
    TEST_IT_NAME("fixes #26 missing-last-line-on-save-if-empty");
  
    /* Create a new empty file */
    char *temp_filename = "/tmp/femto.testdata";

    FILE *temp_fd = fopen( temp_filename, "w" );
    if( ! temp_fd )
    {
        lprintf( LOG_ERROR, "Error creating test file %s", temp_filename );
        return;
    }

    fwrite("1\n", 1, 1, temp_fd);
    fclose( temp_fd );

    /* Read in new file and save it immediately */
    Session *s = fe_init_session( temp_filename );

    if( ! expect_i_eq( 1, s->line_count )) return;

    fe_file_save( s );
    fe_free_session( s );

    temp_fd = fopen(temp_filename, "r");

    if( ! temp_fd )
    {
        lprintf( LOG_ERROR, "Error open file %s", temp_filename );
        return;
    }

    char buffer[8];

    size_t bytes_read = fread(&buffer, 1, sizeof(buffer), temp_fd);
    fclose(temp_fd);
    unlink(temp_filename);
    
    if( ! expect_i_eq( 2, bytes_read )) return;
    if( ! expect_b_eq( "1\n", buffer, 2, bytes_read )) return;

    TEST_OK;
}

void test_suite_session()
{
    TEST_SUITE_NAME("Session");

    TEST_CONTEXT_NAME( "General" );
    test_session_init();

    TEST_CONTEXT_NAME( "Valid load file" );
    test_load_file();
    test_load_file_with_long_lines();
    test_load_empty_file();

    TEST_CONTEXT_NAME( "Valid save file" );
    test_save_new_file();
    test_save_empty_new_file();

    TEST_CONTEXT_NAME( "Valid load+save file" );
    test_save_existing_file_without_change();
    test_save_empty_existing_file_without_change();
    test_truncates_existing_file();
    test_fixup_issue_26();

    TEST_CONTEXT_NAME( "Invalid load/save file" );
    test_invalid_save_file_without_filename();
    test_invalid_load_file_without_filename();
}
