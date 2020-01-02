#include "femto.h"

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

    printf("Read %lu bytes\n", file_size);

    printf("File content ---\n%s\n---\n",content);

    // Get terminal info (buffer resolution)
    // Print file on screen

    printf("Write file to test.txt\n");
    fe_file_save("test.txt", content, file_size);

    // Set terminal mode to raw in order to disable unwanted behaviour
    fe_enable_raw_mode();

    // Restore old terminal mode
    fe_disable_raw_mode();
    // Free resources
    free(content);

    return EXIT_SUCCESS;
}


//
// CURSOR POSITIONING
//

void fe_enable_raw_mode()
{
}

void fe_disable_raw_mode()
{
}
