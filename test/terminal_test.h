#ifndef TERMINAL_TEST_H
#define TERMINAL_TEST_H

#include "test_util.h"
#include <terminal.h>

void test_enable_and_disable_raw_mode_without_error();
void test_get_terminal_size_without_error();
void test_get_cursor_position_without_error();
void test_set_and_get_cursor_position_without_error();
void test_suite_terminal();

#endif
