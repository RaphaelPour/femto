#include "screen.h"

void fe_refresh_screen(Session *s){
    Buffer *screen_buffer = fe_create_buffer();

    fe_append_to_buffer(screen_buffer, ESC_HIDE_CURSOR, strlen(ESC_HIDE_CURSOR));
    fe_append_to_buffer(screen_buffer, ESC_RESET_CURSOR, strlen(ESC_RESET_CURSOR));


    unsigned row;
    TerminalSize ts = s->terminal_size;
    // Show welcome screen when no file is loaded
    if(s->line_count == 0)
    {
        for(row=0; row < ts.rows-1; row++)
        {
            if(row == ts.rows/2)
                fe_append_to_buffer(screen_buffer, "~\t\t\t\t< femto >\x1b[0K\r\n",21);
            else
                fe_append_to_buffer(screen_buffer, "~\x1b[0K\r\n", 7);
        }
    }
    else
    {
        for(row=0;row < ts.rows;row++)
        {
            unsigned line_index = row + s->cursor_position.y;
            if(line_index < s->line_count)
            {
                
                fe_append_to_buffer(screen_buffer, 
                                    s->lines[line_index].content, 
                                    s->lines[line_index].length);
                fe_append_to_buffer(screen_buffer,
                                    ESC_DEL_TO_EOL,
                                    strlen(ESC_DEL_TO_EOL));
                fe_append_to_buffer(screen_buffer, 
                                    NEW_LINE,
                                    strlen(NEW_LINE));
            }
            else
                fe_append_to_buffer(screen_buffer, "~\x1b[0K\r\n", 7);
        }
    }
    
    write(STDOUT_FILENO, screen_buffer->data, screen_buffer->length);

    fe_free_buffer(screen_buffer);
}
