#include "file_io.h"

//
// FILE IO
//

void fe_file_save(char *filename, Buffer *buffer)
{
    FILE *file_handle = fopen(filename, "w");
    
    if(file_handle == NULL)
    {
        perror("open file for write");
        exit(EXIT_FAILURE);
    }

    size_t bytes_written = fwrite(buffer->data, 1, buffer->length, file_handle);

    if(bytes_written<buffer->length)
    {
        printf("Error writing file\n");
        exit(EXIT_FAILURE);
    }

    fclose(file_handle);
}

