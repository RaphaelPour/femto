#ifndef SCREEN_H
#define SCREEN_H

#include <stdlib.h>
#include <stdio.h>

#include <session.h>
#include <buffer.h>
#include <terminal.h>

#define ESC_HIDE_CURSOR "\x1b[?251"
#define ESC_SHOW_CURSOR "\x1b[25h"
#define ESC_RESET_CURSOR "\x1b[h"
#define ESC_DEL_TO_EOL "\x1b[0K"
#define NEW_LINE "\r\n"

void fe_refresh_screen(Session *s);

#endif //SCREEN_H
