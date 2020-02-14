#include "femto.h"
#include <unistd.h>

int main(int argc, char *argv[])
{
    
    // Check arguments
    if(argc != 2)
    {
        printf("usage: femto <file>\n");
        return EXIT_FAILURE;
    }

    char *filename = argv[1];

    // Create session with delivered file
    Session *session= fe_init_session(filename);

    // Set terminal mode to raw in order to disable unwanted behaviour
    fe_enable_raw_mode();

    fe_refresh_screen(session);


    // Restore old terminal mode
    fe_disable_raw_mode();
    
    // Free resources
    fe_free_session(session);

    return EXIT_SUCCESS;
}


