#include <stdlib.h>
#include <string.h>

#include "buffer.h"

Buffer* fe_create_buffer()
{
    Buffer *buf = (Buffer*)malloc(sizeof(Buffer));
    buf->data = NULL;
    buf->length = 0;
    return buf;
}

void fe_append_to_buffer(Buffer *buffer, const char *appendix, unsigned length)
{
    char * resized_data = realloc(buffer->data, buffer->length + length);

    if(!resized_data) return;

    memcpy(resized_data + buffer->length, appendix, length);

    buffer->data = resized_data;
    buffer->length += length;
}

void fe_free_buffer(Buffer *buffer)
{
    free( buffer->data );
    free( buffer );
}
