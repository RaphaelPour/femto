#include "femto_test.h"

static Session* create_session_zero_initialized()
{
    Session *s = (Session*) malloc(sizeof(Session));

    s->filename = NULL;
    s->terminal_size = (TerminalSize){{0},{0}};
    s->cursor_position = (TerminalPosition){{1},{1}};
    s->offset = (TerminalPosition){{0},{0}};
    s->line_count = 0;
    s->lines = 0;
    s->edit_mode = 0;

    return s;
}

static Session* duplicate_session(Session *s)
{
    Session *other = (Session*) malloc(sizeof(Session));

    memcpy((void*)other, (void*) s, sizeof(Session));

    return other;
}


void test_valid_cursor_down_movement()
{
    TEST_IT_NAME("moves cursor downwards as it should. ts:2/2, lc:2, off:0/0, p:1/1 -> py:2");

    Line lines[2] = {
        {.index = 0, .content = "1",.length = 1},
        {.index = 0, .content = "2",.length = 1},
    };

    Session *actualSession = create_session_zero_initialized();
    actualSession->cursor_position.x = 1;
    actualSession->cursor_position.y = 1;
    actualSession->terminal_size.width = 2;
    actualSession->terminal_size.height = 2;
    actualSession->line_count = 2;
    actualSession->content_length = 2;
    actualSession->lines = lines;

    Session *expectedSession = duplicate_session(actualSession);
    expectedSession->cursor_position.y = 2;

    // Move cursor 1 down
    fe_move(actualSession, 0, 1);

    if(!expect_session_equal(actualSession, expectedSession, "Valid downward movement failed.")) return;
    TEST_OK

    free(actualSession);
    free(expectedSession);
}

void test_valid_cursor_up_movement()
{
    TEST_IT_NAME("moves cursor upwards as it should. ts:2/2, lc:2, off:0/0, p:1/2 -> py:1");

    Line lines[2] = {
        {.index = 0, .content = "1",.length = 1},
        {.index = 0, .content = "2",.length = 1},
    };

    Session actualSession = {
        .filename = NULL,
        .cursor_position = {{1},{2}},
        .terminal_size = {{2},{2}},
        .offset = {{0},{0}},
        .line_count = 2,
        .content_length = 2,
        .lines = lines,
        .edit_mode = 0
    };

    Session expectedSession = {
        .filename = NULL,
        .cursor_position = {{1},{1}},
        .terminal_size = {{2},{2}},
        .offset = {{0},{0}},
        .line_count = 2,
        .content_length = 2,
        .lines = lines,
        .edit_mode = 0
    };


    // Move cursor 1 down
    fe_move(&actualSession, 0, -1);

    if(!expect_session_equal(&actualSession, &expectedSession, "Valid upwards movement failed.")) return;
    TEST_OK
}

void test_valid_cursor_left()
{
    TEST_IT_NAME("moves cursor left as it should. ts:3/1, lc:1, off:0/0, p:2/1 -> px:1");

    Line lines[1] = {{.index = 0, .content = "123", .length = 3}};

    Session actualSession = {
        .filename = NULL,
        .cursor_position = {{2},{1}},
        .terminal_size = {{3},{1}},
        .offset = {{0},{0}},
        .line_count = 1,
        .content_length = 3,
        .lines = lines,
        .edit_mode = 0,
    };

    Session expectedSession = {
        .filename = NULL,
        .cursor_position = {{1},{1}},
        .terminal_size = {{3},{1}},
        .offset = {{0},{0}},
        .line_count = 1,
        .content_length = 3,
        .lines = lines,
        .edit_mode = 0,
    };

    fe_move(&actualSession, -1,0);

    if(!expect_session_equal(&actualSession, &expectedSession, "Valid left movement failed.")) return;
    TEST_OK;
}

void test_valid_cursor_right()
{
    TEST_IT_NAME("moves cursor right as it should. ts:3/1, lc:1, off:0/0, p:2/1 -> px:3");

    Line lines[1] = {{.index = 0, .content = "123", .length = 3}};

    Session actualSession = {
        .filename = NULL,
        .cursor_position = {{2},{1}},
        .terminal_size = {{3},{1}},
        .offset = {{0},{0}},
        .line_count = 1,
        .content_length = 3,
        .lines = lines,
        .edit_mode = 0,
    };

    Session expectedSession = {
        .filename = NULL,
        .cursor_position = {{3},{1}},
        .terminal_size = {{3},{1}},
        .offset = {{0},{0}},
        .line_count = 1,
        .content_length = 3,
        .lines = lines,
        .edit_mode = 0,
    };


    fe_move(&actualSession, 1,0);

    if(!expect_session_equal(&actualSession, &expectedSession, "Valid right movement failed.")) return;
    TEST_OK;
}
void test_suite_cursor()
{
    TEST_SUITE_NAME("Cursor");
    test_valid_cursor_down_movement();
    test_valid_cursor_up_movement();
    test_valid_cursor_left();
    test_valid_cursor_right();
}
