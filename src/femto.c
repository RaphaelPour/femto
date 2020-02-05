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
    printf("Open file %s\n", filename);

    // Load file (if exist)
    char *content = NULL;

    size_t file_size = fe_file_load(filename, &content);


    // Set terminal mode to raw in order to disable unwanted behaviour
    fe_enable_raw_mode();


    fe_refresh_screen();


    // Restore old terminal mode
    fe_disable_raw_mode();
    // Free resources
    free(content);

    return EXIT_SUCCESS;
}


