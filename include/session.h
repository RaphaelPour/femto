#ifndef SESSION_H
#define SESSION_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <terminal.h>
#include <buffer.h>
#include <helper.h>

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

    /* Current mode to interpret the keys either literally or as commands */
    unsigned char edit_mode;

    /* Dirty flag */
    bool dirty;
} Session;

Session* fe_init_session(char* filename);
void fe_dump_session(Session *s);
void fe_toggle_mode(Session *s);
void fe_move(Session *s, int x, int y);
void fe_set_filename( Session *s, const char *filename );
bool fe_file_load( Session *s );
bool fe_file_save( Session *s );
void fe_free_session(Session *s);

void fe_insert_char(Session *s, char c);
void fe_remove_char_at_cursor(Session *s);
void fe_insert_line(Session *s);
#endif // SESSION_H
