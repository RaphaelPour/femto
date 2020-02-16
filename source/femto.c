#include "femto.h"

int main(int argc, char *argv[])
{
    lopen("femto.log", LOG_DEBUG);

    /* Argument parsing */
    if(argc != 2)
    {
        printf("usage: femto <file>\n");
        return EXIT_FAILURE;
    }

    char *filename = argv[1];

    /* Create session with delivered file */
    Session *session= fe_init_session(filename);

    /* 
     * Set terminal mode to raw in order to disable 
     * unwanted behaviour
     */
    fe_enable_raw_mode();

    /*
     * CYCLING EXECUTIVE: Read user input, interpret them, render buffer 
     */
    char c;
    int exit_femto = 0;
    while( !exit_femto )
    {
        /* Render the current buffer to the terminal screen*/
        fe_refresh_screen(session);
        
        c = fe_get_user_input();
        lprintf(LOG_INFO, "read user input '%c'", c);
        
        if(c == 27)
        {
            fe_toggle_mode(session);
            lprintf(LOG_INFO, "toggle edit mode: %s", ((session->edit_mode) ? "ON":"OFF"));
            continue;
        }

        if(session->edit_mode)
        {
            /* Manipulate the real buffer */
        }
        else
        {
            switch(c)
            {
                case 'w':
                    fe_move(session,0,-1);
                    break;
                case 'a':
                    fe_move(session,-1,0);
                    break;
                case 's':
                    fe_move(session,0,1);
                    break;
                case 'd':
                    fe_move(session,1,0);
                    break;
                case 'x':
                    exit_femto = 1;
                    lprintf(LOG_INFO, "exited by user");
                    break;
            }
        }

    }

    /* Restore old terminal mode */
    fe_disable_raw_mode();
    
    /* Free resources aka make valgrind happy */
    fe_free_session(session);

    lclose();
    return EXIT_SUCCESS;
}


