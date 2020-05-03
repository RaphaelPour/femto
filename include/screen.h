#ifndef SCREEN_H
#define SCREEN_H

#include <stdlib.h>
#include <stdio.h>

#include <session.h>
#include <buffer.h>
#include <terminal.h>

#define ESC_HIDE_CURSOR "\033[?25l"
#define ESC_SHOW_CURSOR "\033[?25h"
#define ESC_RESET_CURSOR "\033[H"
#define ESC_SET_CURSOR "\033[%d;%dH"
#define ESC_DEL_TO_EOL "\033[0K"
#define NEW_LINE "\r\n"
#define SPARE_LINE "~"
#define WELCOME_LINE "\t\t\t\t< femto >"

char* fe_create_set_cursor_command(Session *s);
void fe_refresh_screen(Session *s);

#endif //SCREEN_H
