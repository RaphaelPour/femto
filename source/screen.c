#include "screen.h"

char* fe_create_set_cursor_command(Session *s)
{
    /* Basic size including command characters only */
    size_t len = 4;
    
    /* Add count of digits for x and y */
    len += DIGITS(s->cursor_position.x) +
           DIGITS(s->cursor_position.y+1);

    /* Add one for the null terminator */
    len++;

    char* buf = (char*) malloc( len+1 );

    snprintf(buf, len, ESC_SET_CURSOR, 
             s->cursor_position.y+1,
             s->cursor_position.x);


    lprintf(LOG_INFO, "set cursor command: x=%d y=%d", s->cursor_position.x, s->cursor_position.y+1);

    return buf;
}

void fe_render_status_bar( Session *s, Buffer *screen_buffer )
{
    char *status_bar = (char*) malloc( s->terminal_size.width );
    memset( status_bar, 0, s->terminal_size.width );

    snprintf( status_bar, s->terminal_size.width, "%s %3d:%-3d", 
              s->filename ? s->filename : "new file",
              s->cursor_position.row,
              s->cursor_position.col );

    fe_append_to_buffer( screen_buffer,
                         STATUS_BAR_COLOR,
                         strlen( STATUS_BAR_COLOR ));

    fe_append_to_buffer( screen_buffer,
                         status_bar,
                         s->terminal_size.width );

    fe_append_to_buffer( screen_buffer,
                         ESC_DEL_TO_EOL,
                         strlen( ESC_DEL_TO_EOL ));

    fe_append_to_buffer( screen_buffer,
                         RESET_COLOR,
                         strlen( RESET_COLOR ));

    fe_append_to_buffer( screen_buffer, 
                         NEW_LINE,
                         strlen( NEW_LINE ));

    free( status_bar );
    
}

void fe_refresh_screen(Session *s){
    Buffer *screen_buffer = fe_create_buffer();

    fe_append_to_buffer(screen_buffer, ESC_HIDE_CURSOR, strlen(ESC_HIDE_CURSOR));
    fe_append_to_buffer(screen_buffer, ESC_RESET_CURSOR, strlen(ESC_RESET_CURSOR));


    unsigned row;
    TerminalSize ts = s->terminal_size;

    fe_render_status_bar(s, screen_buffer );

    // Show welcome screen when no file is loaded
    if(s->line_count == 0)
    {
        for(row=0; row < ts.rows-1; row++)
        {
            fe_append_to_buffer(screen_buffer, 
                                SPARE_LINE, 
                                strlen(SPARE_LINE));
            
            if(row == ts.rows/2){
                fe_append_to_buffer(screen_buffer,
                                    WELCOME_LINE,
                                    strlen(WELCOME_LINE));
            }

            fe_append_to_buffer(screen_buffer,
                                ESC_DEL_TO_EOL,
                                strlen(ESC_DEL_TO_EOL));
            if(row < ts.rows - 1)
                fe_append_to_buffer(screen_buffer, 
                                    NEW_LINE,
                                    strlen(NEW_LINE));
        }
    }
    else
    {
        for(row=0;row < ts.rows-1;row++)
        {
            unsigned line_index = row + s->offset.y;
            if(line_index < s->line_count)
            {
                fe_append_to_buffer(screen_buffer, 
                                    s->lines[line_index].content, 
                                    s->lines[line_index].length);
            }
            else
            {
                fe_append_to_buffer(screen_buffer,
                                    SPARE_LINE,
                                    strlen(SPARE_LINE));
            }

            fe_append_to_buffer(screen_buffer,
                                ESC_DEL_TO_EOL,
                                strlen(ESC_DEL_TO_EOL));

            if(row < ts.rows - 2)
                fe_append_to_buffer(screen_buffer, 
                                    NEW_LINE,
                                    strlen(NEW_LINE));
        }
    }
    
    /* Show cursor on the right position */
    char *command_buffer = fe_create_set_cursor_command(s);
    fe_append_to_buffer(screen_buffer, command_buffer, strlen(command_buffer));
    fe_append_to_buffer(screen_buffer, ESC_SHOW_CURSOR, strlen(ESC_SHOW_CURSOR));
    free(command_buffer);

    write(STDOUT_FILENO, screen_buffer->data, screen_buffer->length);

    fe_free_buffer(screen_buffer);
}
