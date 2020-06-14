#include <stdlib.h>
#include <stdio.h>

#include <terminal.h>

#include "femto_test.h"

void test_enable_and_disable_raw_mode_without_error()
{
    TEST_IT_NAME( "enables and disables raw mode without an error" );

    fe_enable_raw_mode();
    fe_disable_raw_mode();

    TEST_OK
}

void test_get_terminal_size_without_error()
{
    TEST_IT_NAME( "gets terminal size without an error" );

    TerminalSize s = fe_terminal_size();

    if( ! expect_i_gt( s.rows, 0 )) return;
    if( ! expect_i_gt( s.cols, 0 )) return;

    TEST_OK
}

void test_get_cursor_position_without_error()
{
    TEST_IT_NAME( "gets the cursor position without an error" );

    fe_enable_raw_mode();
    TerminalPosition p = fe_get_cursor_position();
    fe_disable_raw_mode();

    if( ! expect_i_eq( p.x, 0 )) return;
    if( ! expect_i_eq( p.y, 0 )) return;

    TEST_OK
}

void test_set_and_get_cursor_position_without_error()
{
    TEST_IT_NAME( "sets and gets a random cursor position without an error" );

    fe_enable_raw_mode();
    
    // Set random cursor position
    TerminalPosition p = fe_get_cursor_position();

    // Get current cursor position
    fe_disable_raw_mode();

    // Check if the cursor posistion is right
    if( ! expect_i_eq( p.x, 0 )) return;
    if( ! expect_i_eq( p.y, 0 )) return;

    TEST_OK
}

void test_suite_terminal()
{
    TEST_SUITE_NAME( "Terminal" );

    test_enable_and_disable_raw_mode_without_error();
    test_get_terminal_size_without_error();
    test_get_cursor_position_without_error();
}


