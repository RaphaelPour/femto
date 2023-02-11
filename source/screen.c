#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <terminal.h>
#include <helper.h>
#include <screen.h>

const char *welcome_message[] = {
    "\t\tfemto ",
    "",
    "\t\t<arrow-keys>    Navigation",
    "\t\t<enter>         New line",
    "\t\t<backspace>     Remove char/line before cursor",
    "\t\t<delete>        Remove char/line after cursor",
    "\t\t<esc>           Quit",
    "\t\t<ctrl+s>        Save",
    NULL
};

char* fe_create_set_cursor_command(Session *s, int offset_x, int offset_y)
{
    /* Basic size including command characters only */
    size_t len = 4;
    
    /* Add count of digits for x and y */
    len += DIGITS(s->cursor_position.x + offset_x ) +
           DIGITS(s->cursor_position.y + 1 + offset_y );

    /* Add one for the null terminator */
    len++;

    char* buf = (char*) malloc( len+1 );

    snprintf(buf, len, ESC_SET_CURSOR, 
             s->cursor_position.y + 1 + offset_y,
             s->cursor_position.x + offset_x );


    lprintf(LOG_INFO, "set cursor command: x=%d y=%d", s->cursor_position.x, s->cursor_position.y+1);

    return buf;
}

Buffer* fe_generate_prompt_status_bar( Session *s, char *prompt, char *input )
{
    Buffer *buffer = fe_create_buffer();
    char *status_bar = (char*) malloc( s->terminal_size.width );
    memset( status_bar, 0, s->terminal_size.width );

    snprintf( status_bar, s->terminal_size.width, "%s %s", prompt, input );

    fe_append_to_buffer( buffer,
                         STATUS_BAR_COLOR,
                         strlen( STATUS_BAR_COLOR ));

    fe_append_to_buffer( buffer,
                         status_bar,
                         s->terminal_size.width );

    fe_append_to_buffer( buffer,
                         ESC_DEL_TO_EOL,
                         strlen( ESC_DEL_TO_EOL ));

    fe_append_to_buffer( buffer,
                         RESET_COLOR,
                         strlen( RESET_COLOR ));

    fe_append_to_buffer( buffer, 
                         NEW_LINE,
                         strlen( NEW_LINE ));

    free( status_bar );
    
    return buffer;

}

Buffer* fe_generate_default_status_bar( Session *s )
{
    Buffer *buffer = fe_create_buffer();
    char *status_bar = (char*) malloc( s->terminal_size.width );
    memset( status_bar, 0, s->terminal_size.width );

    snprintf( status_bar, s->terminal_size.width, "%1s%s %3d:%-3d", 
              s->dirty ? "*" : "",
              s->filename ? s->filename : "new file",
              s->cursor_position.row + s->offset.row,
              s->cursor_position.col + s->offset.col);

    fe_append_to_buffer( buffer,
                         STATUS_BAR_COLOR,
                         strlen( STATUS_BAR_COLOR ));

    fe_append_to_buffer( buffer,
                         status_bar,
                         s->terminal_size.width );

    fe_append_to_buffer( buffer,
                         ESC_DEL_TO_EOL,
                         strlen( ESC_DEL_TO_EOL ));

    fe_append_to_buffer( buffer,
                         RESET_COLOR,
                         strlen( RESET_COLOR ));

    fe_append_to_buffer( buffer, 
                         NEW_LINE,
                         strlen( NEW_LINE ));

    free( status_bar );
    
    return buffer;
}

void fe_refresh_screen(Session *s, Buffer *status_bar){
    Buffer *screen_buffer = fe_create_buffer();

    fe_append_to_buffer(screen_buffer, ESC_HIDE_CURSOR, strlen(ESC_HIDE_CURSOR));
    fe_append_to_buffer(screen_buffer, ESC_RESET_CURSOR, strlen(ESC_RESET_CURSOR));


    unsigned row;
    unsigned line_no_len = DIGITS( s->line_count ) +1;
    TerminalSize ts = s->terminal_size;

    if( status_bar )
    {
        fe_append_to_buffer( screen_buffer,
                             status_bar->data,
                             status_bar->length );
    }
    else
    {
        Buffer *default_status_bar = fe_generate_default_status_bar( s );
        fe_append_to_buffer( screen_buffer,
                             default_status_bar->data,
                             default_status_bar->length );
        fe_free_buffer( default_status_bar );
    }

    /* Show welcome screen on new file with no content */
    if(s->line_count == 1 && s->lines[0].length == 0 && !s->dirty)
    {
        int welcome_index = 0;
        /* Overstep status bar */
        for(row=1; row < ts.rows-1; row++)
        {
            fe_append_to_buffer(screen_buffer, 
                                SPARE_LINE, 
                                strlen(SPARE_LINE));
            
            if(row > ts.rows/2){
                if( welcome_message[welcome_index] )
                {
                    fe_append_to_buffer(screen_buffer,
                                        welcome_message[welcome_index],
                                        strlen(welcome_message[welcome_index]));
                    welcome_index++;
                }
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
        char *line_no = malloc( sizeof(char) * (line_no_len+1));
        for(row=0;row < ts.rows-1;row++)
        {
            unsigned line_index = row + s->offset.y;
            

            if(line_index < s->line_count)
            {
                /* highlight the line of code if the filename is present, 
                 * otherwise the pointer to the default line (s->lines[line_index]) is returned
                */
                Line *line = fe_highlight( s->highlighter, (Buffer*) &s->lines[line_index] );

                /* Add line number */
                snprintf(
                    line_no,
                    line_no_len+1,
                    "%*d ",
                    line_no_len-1,
                    line_index+1);
                fe_append_to_buffer( screen_buffer, line_no, line_no_len + 1 );
                fe_append_to_buffer(screen_buffer, 
                                    (line->data), 
                                    line->length);
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
        free( line_no );
    }
    
    /* Show cursor on the right position */
    char *command_buffer = fe_create_set_cursor_command(s, line_no_len, 0 );
    fe_append_to_buffer(screen_buffer, command_buffer, strlen(command_buffer));
    fe_append_to_buffer(screen_buffer, ESC_SHOW_CURSOR, strlen(ESC_SHOW_CURSOR));
    free(command_buffer);

    write(STDOUT_FILENO, screen_buffer->data, screen_buffer->length);

    fe_free_buffer(screen_buffer);
}
