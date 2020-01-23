#include "screen.h"



void fe_refresh_screen(){
    buffer *screen_buffer = fe_create_buffer();

    fe_append_to_buffer(screen_buffer, ESC_HIDE_CURSOR, strlen(ESC_HIDE_CURSOR));
    fe_append_to_buffer(screen_buffer, ESC_RESET_CURSOR, strlen(ESC_RESET_CURSOR));

    terminal_size ts = fe_terminal_size();
    unsigned row;

    for(row=0; row < ts.rows-1; row++)
        if(row == ts.rows/2)
            fe_append_to_buffer(screen_buffer, "~\t\t\t\t< femto >\x1b[0K\r\n",21);
        else
            fe_append_to_buffer(screen_buffer, "~\x1b[0K\r\n", 7);
    
    write(STDOUT_FILENO, screen_buffer->data, screen_buffer->length);

    fe_free_buffer(screen_buffer);
}
