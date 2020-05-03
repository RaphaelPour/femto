#include "femto_test.h"

static Session* create_session_zero_initialized()
{
    Session *s = (Session*) malloc(sizeof(Session));

    s->filename = NULL;
    s->terminal_size = (TerminalSize){{0},{0}};
    s->cursor_position = (TerminalPosition){{1},{1}};
    s->offset = (TerminalPosition){{0},{0}};
    s->line_count = 0;
    s->content_length = 0;
    s->lines = NULL;
    s->edit_mode = 0;

    return s;
}

static Session* create_session_perfect_fit_by_input(int line_count, char *lines[])
{
    Session *s = create_session_zero_initialized();

    s->lines = calloc(line_count, sizeof(Line));

    int i, total_length = 0, longest_line_length = 0;
    for(i=0;i<line_count;i++)
    {
        int len = strlen( lines[i] );
        s->lines[i].content = (char*)malloc( len );
        memcpy(s->lines[i].content, lines[i], len);

        s->lines[i].index = i;
        s->lines[i].length = len;

        total_length += len;
        longest_line_length = MAX(longest_line_length, len);
    }

    s->line_count = line_count;
    s->content_length = total_length;

    s->terminal_size.height = line_count;
    s->terminal_size.width = longest_line_length;

    return s;
}

static Session* duplicate_session(Session *s)
{
    Session *other = (Session*) malloc(sizeof(Session));

    memcpy( (void*)other, (void*) s, sizeof(Session));

    other->filename = NULL;
    other->lines = NULL;

    if( s->filename )
    {
        other->filename = (char*) malloc( strlen( s->filename ) + 1 );
        strcpy( other->filename, s->filename );
        other->filename[strlen( s->filename )] = '\0';
    }

    if( s->line_count > 0 )
    {
        other->lines = (Line*) malloc( s->line_count * sizeof(Line) );
        memcpy( other->lines, s->lines, s->line_count * sizeof(Line) );

        for( int i = 0; i < s->line_count; i++ )
        {
            other->lines[i].content = (char*) malloc( s->lines[i].length );
            memcpy( other->lines[i].content, s->lines[i].content, s->lines[i].length );
            
        }
    }

    return other;
}


void test_valid_cursor_down_movement()
{
    TEST_IT_NAME("moves cursor downwards. ts:2/2, lc:2, off:0/0, p:1/1 -> py:2");

    char *lines[] = {
        "1","2"
    };

    /*
     * x  = Cursor
     * -> = Movement
     *
     * x  -down-> 1
     * 2          x
     */

    Session *actualSession = create_session_perfect_fit_by_input(2,lines);
    actualSession->cursor_position.x = 1;
    actualSession->cursor_position.y = 1;

    Session *expectedSession = duplicate_session( actualSession );
    expectedSession->cursor_position.y = 2;

    // Move cursor 1 down
    fe_move(actualSession, 0, 1);

    if(!expect_session_equal(expectedSession, actualSession )) goto cleanup;

    TEST_OK;

cleanup:
    fe_free_session( actualSession );
    fe_free_session( expectedSession );

}

void test_valid_cursor_up_movement()
{
    TEST_IT_NAME("moves cursor upwards. ts:2/2, lc:2, off:0/0, p:1/2 -> py:1");

    char *lines[] = {
        "1","2"
    };

    /*
     * 1  -up-> x
     * x        2
     */
    Session *actualSession = create_session_perfect_fit_by_input(2,lines);
    actualSession->cursor_position.x = 1;
    actualSession->cursor_position.y = 2;

    Session *expectedSession = duplicate_session( actualSession );
    expectedSession->cursor_position.y = 1;

    // Move cursor 1 up
    fe_move(actualSession, 0, -1);

    if(!expect_session_equal(expectedSession, actualSession )) goto cleanup;
    
    TEST_OK;

cleanup:
    fe_free_session( actualSession );
    fe_free_session( expectedSession );
}

void test_valid_cursor_left()
{
    TEST_IT_NAME("moves cursor left. ts:3/1, lc:1, off:0/0, p:2/1 -> px:1");

    char *lines[] = {"123"};

    /*
     * 1x3 -left-> x23
     */
    Session *actualSession = create_session_perfect_fit_by_input(1,lines);
    actualSession->cursor_position.x = 2;
    actualSession->cursor_position.y = 1;

    Session *expectedSession = duplicate_session( actualSession );
    expectedSession->cursor_position.x = 1;    

    fe_move(actualSession, -1,0);

    if(!expect_session_equal(expectedSession, actualSession )) goto cleanup;
    
    TEST_OK;

cleanup:
    fe_free_session( actualSession );
    fe_free_session( expectedSession );
}

void test_valid_cursor_right()
{
    TEST_IT_NAME("moves cursor right. ts:3/1, lc:1, off:0/0, p:2/1 -> px:3");

    char *lines[] = {"123"};

    /*
     * 1x3 -right-> 12x
     */
    Session *actualSession = create_session_perfect_fit_by_input(1,lines);
    actualSession->cursor_position.x = 2;
    actualSession->cursor_position.y = 1;

    Session *expectedSession = duplicate_session( actualSession );
    expectedSession->cursor_position.x = 3;    

    fe_move(actualSession, 1,0);

    if(!expect_session_equal( expectedSession, actualSession )) goto cleanup;
    
    TEST_OK;

cleanup:
    fe_free_session( actualSession );
    fe_free_session( expectedSession );
}

void test_valid_cursor_circle()
{
    TEST_IT_NAME("moves cursor circular and ends at the origin. ts:2/2, lc:2, off:0/0, p:1/1 -> p:1/1");

    char *lines[] = { "AB","CD"};

    /*
     * x2 -right-> 1x -down-> 12 -left-> 12 -up-> x2 
     * 34          34         3x         x4       34
     */
    Session *actualSession = create_session_perfect_fit_by_input(2,lines);
    actualSession->cursor_position.x = 1;
    actualSession->cursor_position.y = 1;

    Session *expectedSession = duplicate_session( actualSession );

    // Move right
    fe_move(actualSession, 1,0);

    // Move down
    fe_move(actualSession, 0,1);

    // Move left
    fe_move(actualSession, -1,0);

    // Move up
    fe_move(actualSession, 0,-1);
    
    if(!expect_session_equal( expectedSession, actualSession )) goto cleanup;
    
    TEST_OK;
    
cleanup:
    fe_free_session( actualSession );
    fe_free_session( expectedSession );
}

void test_invalid_cursor_down_movement()
{
    TEST_IT_NAME("doesn't move cursor downwards (end of buffer). ts:2/2, lc:2, off:0/0, p:1/2 -> no change");


    char *lines[] = {
        "1","2"
    };


    /*
     * 1 -down-> 1
     * x         x
     */
    Session *actualSession = create_session_perfect_fit_by_input(2,lines);
    actualSession->cursor_position.x = 1;
    actualSession->cursor_position.y = 2;

    // We expect no change of the original session
    Session *expectedSession = duplicate_session( actualSession );

    // Move cursor 1 down
    fe_move(actualSession, 0, 1);

    if(!expect_session_equal( expectedSession, actualSession )) goto cleanup;

    TEST_OK;

cleanup:
    fe_free_session( actualSession );
    fe_free_session( expectedSession );
}


void test_invalid_cursor_up_movement()
{
    TEST_IT_NAME("doesn't move cursor upwards (end of buffer). ts:2/2, lc:2, off:0/0, p:1/1 -> no change");

    char *lines[] = {
        "1","2"
    };

    /*
     * x -up-> x
     * 2       2
     */
    Session *actualSession = create_session_perfect_fit_by_input(2,lines);
    actualSession->cursor_position.x = 1;
    actualSession->cursor_position.y = 1;

    // We expect no change of the original session
    Session *expectedSession = duplicate_session( actualSession );

    // Move cursor 1 down
    fe_move(actualSession, 0,-1);

    if(!expect_session_equal( expectedSession, actualSession )) goto cleanup;
    
    TEST_OK;

cleanup:
    fe_free_session( actualSession );
    fe_free_session( expectedSession );
}


void test_invalid_cursor_left()
{
    TEST_IT_NAME("doesn't move cursor left (end of buffer). ts:3/1, lc:1, off:0/0, p:1/1 -> no change");

    char *lines[] = {"123"};

    /*
     * x23 -left-> x23
     */

    Session *actualSession = create_session_perfect_fit_by_input(1,lines);
    actualSession->cursor_position.x = 1;
    actualSession->cursor_position.y = 1;

    Session *expectedSession = duplicate_session( actualSession );

    fe_move(actualSession, -1,0);

    if(!expect_session_equal(expectedSession, actualSession )) goto cleanup;
    
    TEST_OK;

cleanup:
    fe_free_session( actualSession );
    fe_free_session( expectedSession );
}

void test_invalid_cursor_right()
{
    TEST_IT_NAME("doesn't move cursor right (end of buffer). ts:3/1, lc:1, off:0/0, p:3/1 -> no change");

    char *lines[] = {"1"};

    /*
     * 1x -right-> 1x
     *
     * Note that the terminal has to have a one char wider width since the
     * cursor can get behind the last char and this will cause the width offset
     * to increase.
     */
    Session *actualSession = create_session_perfect_fit_by_input(1,lines);
    actualSession->terminal_size.width += 1;
    actualSession->cursor_position.x = 2;
    actualSession->cursor_position.y = 1;

    Session *expectedSession = duplicate_session( actualSession );

    fe_move(actualSession, 1,0);

    if(!expect_session_equal( expectedSession, actualSession )) goto cleanup;
    
    TEST_OK;

cleanup:
    fe_free_session( actualSession );
    fe_free_session( expectedSession );
}

void test_valid_cursor_down_offset_movement()
{
    lprintf(LOG_INFO, "increases row offset on downward movement");
    TEST_IT_NAME("increases row offset on downward movement");
    
    char *lines[] = {
        "1",
        "2",
        "3"
    };

    /*
     * Create session which perfectly fits for three lines and decrease
     * termional height to two to test if the row offset increases when
     * the cursor would otherwise exit the visible screen buffer.
     */
    Session *actualSession = create_session_perfect_fit_by_input(3, lines);
    actualSession->terminal_size.height = 2;

    Session *expectedSession = duplicate_session( actualSession );
    expectedSession->offset.row = 1;
    expectedSession->cursor_position.y = 2;

    fe_move(actualSession, 0, 1);
    fe_move(actualSession, 0, 1);

    if(!expect_session_equal( expectedSession, actualSession)) goto cleanup;

    TEST_OK;

cleanup:
    fe_free_session( actualSession );
    fe_free_session( expectedSession );
}

void test_valid_up_offset_movement()
{
    lprintf(LOG_INFO, "decreases only row offset on updward movement");
    TEST_IT_NAME("decreases only row offset on updward movement");


    char *lines[] = {
        "1",
        "2",
        "3"
    };

    Session *actualSession = create_session_perfect_fit_by_input(3, lines);
    actualSession->terminal_size.height = 2;
    actualSession->offset.row = 1;

    Session *expectedSession = duplicate_session( actualSession );
    expectedSession->offset.row = 0;

    fe_move(actualSession,0,-1);

    if(!expect_session_equal( expectedSession, actualSession)) goto cleanup;
    
    TEST_OK;

cleanup:
    fe_free_session( actualSession );
    fe_free_session( expectedSession );
}

void test_valid_cursor_up_offset_movement()
{
    lprintf(LOG_INFO, "decreases row offset on upward and up movement");
    TEST_IT_NAME("decreases row offset on upward movement");
    char *lines[] = {
        "1",
        "2",
        "3"
    };

    /*
     * Create session which perfectly fits for three lines and decrease
     * termional height to two to test if the row offset increases when
     * the cursor would otherwise exit the visible screen buffer.
     */
    Session *actualSession = create_session_perfect_fit_by_input(3, lines);
    actualSession->terminal_size.height = 2;
    actualSession->cursor_position.y = 2;
    actualSession->offset.row = 1;
    actualSession->offset.col = 0;
    
    Session *expectedSession = duplicate_session( actualSession );
    expectedSession->offset.row = 0;
    expectedSession->cursor_position.y = 1;
    
    fe_move(actualSession, 0, -1);
    fe_move(actualSession, 0, -1);

    if(!expect_session_equal( expectedSession, actualSession )) goto cleanup;
    
    TEST_OK;

cleanup:
    fe_free_session( actualSession );
    fe_free_session( expectedSession );
}

void test_valid_cursor_down_up_offset_movement()
{

    TEST_IT_NAME("changes row offset on down and up movement");
    
    char *lines[] = {
        "1",
        "2",
    };

    /*
     * Create session which perfectly fits for three lines and decrease
     * termional height to two to test if the row offset increases when
     * the cursor would otherwise exit the visible screen buffer.
     */
    Session *actualSession = create_session_perfect_fit_by_input(2, lines);
    actualSession->terminal_size.height= 1;

    Session *expectedSession = duplicate_session( actualSession );

    fe_move(actualSession, 0, 1);

    fe_move(actualSession, 0, -1);

    if(!expect_session_equal( expectedSession, actualSession )) goto cleanup;
    
    TEST_OK;

cleanup:
    fe_free_session( actualSession );
    fe_free_session( expectedSession );
}

void test_valid_char_insertion(){
    
    TEST_IT_NAME("inserts a char in a new session");
    
    /*
     * Create session which perfectly fits for two characters in one line.
     * When one character will be inserted at the end, the expected session
     * should have an increased line length and cursor x position by one.
     */
    char input = '#';
    Session *actualSession = fe_init_session( NULL );
    
    fe_insert_char( actualSession, input );

    if( ! expect_b_eq( &input, actualSession->lines[0].content, 1, actualSession->lines[0].length )) goto cleanup;
    
    TEST_OK;
cleanup:
    fe_free_session( actualSession );
}

void test_valid_line_insertion(){
    TEST_IT_NAME("insert an emtpy line in a new session");
    Session *actualSession = fe_init_session( NULL );

    fe_insert_line( actualSession );

    if( ! expect_i_eq( 2, actualSession->line_count )) goto cleanup;

    TEST_OK;

cleanup:
    fe_free_session( actualSession );
}

void test_valid_line_in_text_insertion(){
    TEST_IT_NAME("insert an emtpy line in the middle of a line");


    char *linesGiven[] = {
        "12"
    };
    Session *actualSession = create_session_perfect_fit_by_input(1, linesGiven);

    fe_move(actualSession, 1, 0 );

    if( ! expect_i_eq( 2, actualSession->cursor_position.x )) goto cleanup;
    if( ! expect_i_eq( 1, actualSession->cursor_position.y )) goto cleanup;

    fe_insert_line( actualSession );

    if( ! expect_i_eq( 2, actualSession->line_count )) goto cleanup;
    if( ! expect_b_eq( "1", actualSession->lines[0].content, 1, actualSession->lines[0].length )) goto cleanup;
    if( ! expect_b_eq( "2", actualSession->lines[1].content, 1, actualSession->lines[1].length )) goto cleanup;
    
    
    TEST_OK;

cleanup:
    fe_free_session( actualSession );
}

void test_valid_char_insertion_and_remove(){
    
    TEST_IT_NAME("inserts and removes a char in a new session");
    
    /*
     * Create session which perfectly fits for two characters in one line.
     * When one character will be inserted at the end, the expected session
     * should have an increased line length and cursor x position by one.
     */

    char input = '#';
    Session *actualSession = fe_init_session( NULL );
    Session *expectedSession = duplicate_session( actualSession );
    
    fe_insert_char( actualSession, input );
    fe_remove_char_at_cursor( actualSession );

    if( ! expect_session_equal( expectedSession, actualSession )) goto cleanup;

    TEST_OK;

cleanup:
    fe_free_session( actualSession );
    fe_free_session( expectedSession );
}

void test_valid_line_insertion_and_remove(){
    TEST_IT_NAME("insert and remove an emtpy line in a new session");
    
    Session *actualSession = fe_init_session( NULL );
    Session *expectedSession = duplicate_session( actualSession );

    fe_insert_line( actualSession );
    fe_remove_char_at_cursor( actualSession );

    if( ! expect_session_equal( expectedSession, actualSession )) goto cleanup;

    TEST_OK;

cleanup:
    fe_free_session( actualSession );
    fe_free_session( expectedSession );
}

void test_suite_cursor()
{
    TEST_SUITE_NAME( "Cursor" );

    TEST_CONTEXT_NAME("Valid moves");
    test_valid_cursor_down_movement();
    test_valid_cursor_up_movement();
    test_valid_cursor_left();
    test_valid_cursor_right();
    test_valid_cursor_circle();

    TEST_CONTEXT_NAME("Invalid moves");
    test_invalid_cursor_down_movement();
    test_invalid_cursor_up_movement();
    test_invalid_cursor_left();
    test_invalid_cursor_right();

    TEST_CONTEXT_NAME("Valid moves with row offset");
    test_valid_up_offset_movement();
    test_valid_cursor_down_up_offset_movement();
    test_valid_cursor_up_offset_movement();
    
    test_valid_cursor_down_offset_movement();

    TEST_CONTEXT_NAME("Valid insertion");
    test_valid_char_insertion();
    test_valid_line_insertion();
    test_valid_line_in_text_insertion();
    test_valid_char_insertion_and_remove();
    test_valid_line_insertion_and_remove();

}
