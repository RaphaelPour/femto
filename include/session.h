#ifndef SESSION_H
#define SESSION_H

#include <stdlib.h>
#include <stdio.h>
#include <terminal.h>
#include <buffer.h>

typedef struct{
    /* Line number beginning with zero.
     * TODO: This variable may be useless while the line itself
     *       is only used within a line array which is indexed by design
     */
    size_t index;

    /* Raw content of the line excluding new-line and null-terminator */
    char *content;

    /* Length of the content */
    size_t length;
}Line;

typedef struct {
    /* Name of the read file */
    const char *filename;

    /* Dimensions of the terminal */
    TerminalSize terminal_size;

    /* Position on the screen */
    TerminalPosition cursor_position;

    /* Offset between file content and screen */
    TerminalPosition offset;

    /* Array of lines which hold the content */
    Line *lines;
    size_t line_count;

    /* Total length of all line lengths combined */
    size_t content_length;
} Session;

Session* fe_init_session(const char* filename);
void fe_file_load(const char *filename, Session *s);
void fe_free_session(Session *s);

#endif // SESSION_H
