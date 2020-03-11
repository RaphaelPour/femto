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
    s->lines = 0;
    s->edit_mode = 0;

    return s;
}

static Session* create_session_perfect_fit_by_input(int line_count, char *lines[])
{
    Session *s = create_session_zero_initialized();

    s->lines = calloc(line_count, sizeof(Line*));

    int i, total_length = 0, longest_line_length = 0;
    for(i=0;i<line_count;i++)
    {
        int len = strlen(lines[i]);
        s->lines[i].content = (char*)malloc(len);
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

static void clean_up_session(Session *s)
{
    if(s->filename) free(s->filename);

    /*
    if(s->lines)
    {   
        int i;
        for(i=0;i<s->line_count;i++)
            if(s->lines[i].content)
                free(s->lines[i].content);
        
        free(s->lines);
    }*/

    free(s);
}

static Session* duplicate_session(Session *s)
{
    Session *other = (Session*) malloc(sizeof(Session));

    memcpy((void*)other, (void*) s, sizeof(Session));

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

    Session *expectedSession = duplicate_session(actualSession);
    expectedSession->cursor_position.y = 2;

    // Move cursor 1 down
    fe_move(actualSession, 0, 1);

    if(!expect_session_equal(expectedSession, actualSession, "Valid downward movement failed.")) return;
    TEST_OK

    clean_up_session(actualSession);
    clean_up_session(expectedSession);
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

    Session *expectedSession = duplicate_session(actualSession);
    expectedSession->cursor_position.y = 1;

    // Move cursor 1 up
    fe_move(actualSession, 0, -1);

    if(!expect_session_equal(expectedSession, actualSession, "Valid upwards movement failed.")) return;
    TEST_OK
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

    Session *expectedSession = duplicate_session(actualSession);
    expectedSession->cursor_position.x = 1;    

    fe_move(actualSession, -1,0);

    if(!expect_session_equal(expectedSession, actualSession, "Valid left movement failed.")) return;
    TEST_OK;
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

    Session *expectedSession = duplicate_session(actualSession);
    expectedSession->cursor_position.x = 3;    

    fe_move(actualSession, 1,0);

    if(!expect_session_equal( expectedSession, actualSession, "Valid right movement failed.")) return;
    TEST_OK;
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

    Session *expectedSession = duplicate_session(actualSession);

    // Move right
    fe_move(actualSession, 1,0);

    // Move down
    fe_move(actualSession, 0,1);

    // Move left
    fe_move(actualSession, -1,0);

    // Move up
    fe_move(actualSession, 0,-1);
    
    if(!expect_session_equal( expectedSession, actualSession, "Circular movement failed.")) return;
    TEST_OK;
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
    Session *expectedSession = duplicate_session(actualSession);

    // Move cursor 1 down
    fe_move(actualSession, 0, 1);

    if(!expect_session_equal( expectedSession, actualSession, "Inalid downward movement failed.")) return;
    TEST_OK

    free(actualSession);
    free(expectedSession);
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
    Session *expectedSession = duplicate_session(actualSession);

    // Move cursor 1 down
    fe_move(actualSession, 0,-1);

    if(!expect_session_equal( expectedSession, actualSession, "Inalid upward movement failed.")) return;
    TEST_OK

    free(actualSession);
    free(expectedSession);
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

    Session *expectedSession = duplicate_session(actualSession);

    fe_move(actualSession, -1,0);

    if(!expect_session_equal(expectedSession, actualSession, "Invalid left movement failed.")) return;
    TEST_OK;
}

void test_invalid_cursor_right()
{
    TEST_IT_NAME("doesn't move cursor right (end of buffer). ts:3/1, lc:1, off:0/0, p:3/1 -> no change");

    char *lines[] = {"123"};

    /*
     * 12x -right-> 12x
     */
    Session *actualSession = create_session_perfect_fit_by_input(1,lines);
    actualSession->cursor_position.x = 3;
    actualSession->cursor_position.y = 1;

    Session *expectedSession = duplicate_session(actualSession);

    fe_move(actualSession, 1,0);

    if(!expect_session_equal( expectedSession, actualSession, "Invalid right movement failed.")) return;
    TEST_OK;
}
void test_suite_cursor()
{
    TEST_SUITE_NAME("Cursor");
    test_valid_cursor_down_movement();
    test_valid_cursor_up_movement();
    test_valid_cursor_left();
    test_valid_cursor_right();
    test_valid_cursor_circle();
    test_invalid_cursor_down_movement();
    test_invalid_cursor_up_movement();
    test_invalid_cursor_left();
    test_invalid_cursor_right();
}
