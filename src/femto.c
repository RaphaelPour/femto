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

    size_t file_size = fe_read_file(filename, content);

    printf("Read %lu bytes\n", file_size);

    // Get terminal info (buffer resolution)
    // Print file on screen

    return EXIT_SUCCESS;
}

size_t fe_read_file(char *filename, char *content)
{
    FILE *file_handle = fopen(filename, "r");
    size_t file_size = 0;

    // TODO: If file doesn't exist, just return zero. This ensures that
    //       new files can also be created. Ensure that the caller can
    //       handle this (content will still be a null pointer)
    if(file_handle == NULL)
    {
        perror("open file");
        exit(EXIT_FAILURE);
    }

    // 
    // Get the size of the file
    //

    // Set the file pointer to the very end
    if(fseek(file_handle, 0, SEEK_END) == -1)
    {
        perror("seek file end");
        exit(EXIT_FAILURE);
    }

    // Get the file pointers position which is equal to the files size
    file_size = ftell(file_handle);

    if(file_size == -1)
    {
        perror("tell file size");
        exit(EXIT_FAILURE);
    }

    // Put file pointer back to the beginning
    rewind(file_handle);

    //
    // Read file
    //
    
    // Allocate memory for the files content
    content = (char*) malloc(sizeof(char) * file_size );

    if(content == NULL)
    {
        perror("allocate memory for files content");
        exit(EXIT_FAILURE);
    }

    size_t read_bytes = fread(content,1,file_size, file_handle);

    // Regarding to fread(3) an error occured if the return value is less than the
    // item count. In our case, the item count is the count of bytes (=file size)
    if(read_bytes<file_size)
    {
        printf("Error reading file\n");
        exit(EXIT_FAILURE);
    }

    // Finish file operation by closing the file resource
    fclose(file_handle);

    return file_size;
}
