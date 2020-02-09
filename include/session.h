#ifndef SESSION_H
#define SESSION_H

#include <stdlib.h>
#include <stdio.h>
#include <terminal.h>
#include <buffer.h>

typedef struct{
    size_t index;
    char *content;
    size_t length;
}Line;

typedef struct {
    const char *filename;
    TerminalSize terminal_size;
    TerminalPosition cursor_position;
    Line *lines;
    size_t line_count;
    size_t content_length;
} Session;

Session* fe_init_session(const char* filename);
void fe_file_load(const char *filename, Session *s);
void fe_free_session(Session *s);

#endif // SESSION_H
