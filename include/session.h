#ifndef SESSION_H
#define SESSION_H

#include <terminal.h>

typedef struct{
    size_t index;
    size_t length;
}Line;

typedef struct {
    char *filename;
    TerminalSize size;
    TerminalPosition position;
    Line *lines;
} Session;

Session* fe_init_session();
void fe_free_session(Session *s);

#endif // SESSION_H
