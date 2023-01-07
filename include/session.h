#ifndef SESSION_H
#define SESSION_H

#include <unistd.h>
#include <stdbool.h>
#include <terminal.h>
#include <buffer.h>
#include <highlight.h>

typedef Buffer Line;

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

    /* Struct used for highlighting */
    Highlighter *highlighter;

    /* Dirty flag */
    bool dirty;
} Session;

Session* fe_init_session( char* filename );
void fe_dump_session( Session *s );
void fe_move( Session *s, int x, int y );
void fe_move_top( Session *s );
void fe_move_bottom( Session *s );
void fe_move_page_up( Session *s );
void fe_move_page_down( Session *s );
void fe_set_filename( Session *s, const char *filename );
void fe_free_session( Session *s );

void fe_insert_char( Session *s, char c );
void fe_remove_char_at_cursor( Session *s );
void fe_remove_char_after_cursor( Session *s );
void fe_insert_line( Session *s );
#endif // SESSION_H
