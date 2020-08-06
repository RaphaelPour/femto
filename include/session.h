#ifndef SESSION_H
#define SESSION_H

#include <unistd.h>
#include <stdbool.h>
#include <terminal.h>

typedef struct{
    /* Raw content of the line excluding new-line and null-terminator */
    char *content;

    /* Length of the content */
    size_t length;
}Line;

typedef struct {
    /* Name of the read file */
    char *filename;

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

    /* Dirty flag */
    bool dirty;
} Session;

Session* fe_init_session(char* filename);
void fe_dump_session(Session *s);
void fe_move(Session *s, int x, int y);
void fe_set_filename( Session *s, const char *filename );
void fe_free_session(Session *s);

void fe_insert_char(Session *s, char c);
void fe_remove_char_at_cursor(Session *s);
void fe_remove_char_after_cursor(Session *s);
void fe_insert_line(Session *s);
#endif // SESSION_H
