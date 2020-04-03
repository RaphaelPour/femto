#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <terminal.h>
#include <screen.h>
#include <helper.h>

#ifndef BUILD_DATE
#define BUILD_DATE "fix your makefile"
#endif

#ifndef BUILD_VERSION
#define BUILD_VERSION "fix your makefile"
#endif

int main(int argc, char *argv[])
{

    /* Argument parsing */
    if(argc != 2)
    {
        printf("usage: femto <file> | -v\n");
        return EXIT_FAILURE;
    }

    if(strcmp(argv[1],"-v") == 0)
    {
        printf("BuildDate: %s\n", BUILD_DATE);
        printf("BuildVersion: %s\n", BUILD_VERSION);
        return EXIT_SUCCESS;
    }

    lopen("femto.log", LOG_DEBUG);
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
    int c;
    int exit_femto = 0;
    while( !exit_femto )
    {
        /* Render the current buffer to the terminal screen*/
        fe_refresh_screen(session);

        c = fe_get_user_input();
        lprintf(LOG_INFO, "read user input '%c' (%d)", c,c);

        switch(c)
        {
            case UP:
                fe_move(session,0,-1);
                break;
            case LEFT:
                fe_move(session,-1,0);
                break;
            case DOWN:
                fe_move(session,0,1);
                break;
            case RIGHT:
                fe_move(session,1,0);
                break;
            case ESC:
                exit_femto = 1;
                lprintf(LOG_INFO, "exited by user");
                break;
        }

    }

    /* Restore old terminal mode */
    fe_disable_raw_mode();

    /* Free resources aka make valgrind happy */
    fe_free_session(session);

    lclose();
    return EXIT_SUCCESS;
}


