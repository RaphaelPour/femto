#ifndef BUFFER_H
#define BUFFER_H
#include <stdlib.h>
#include <string.h>


// Screen buffer
typedef struct
{
    char *data;
    unsigned length;
}Buffer;

Buffer* fe_create_buffer();
void fe_append_to_buffer(Buffer *buffer, const char *appendix, unsigned length);
void fe_free_buffer(Buffer *buffer);
#endif // BUFFER_H
