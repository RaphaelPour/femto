#include "buffer.h"

buffer* fe_create_buffer()
{
    buffer *buf = (buffer*)malloc(sizeof(buffer));
    buf->data = NULL;
    buf->length = 0;
    return buf;
}

void fe_append_to_buffer(buffer *buffer, const char *appendix, unsigned length)
{
    char * resized_data = realloc(buffer->data, buffer->length + length);

    if(!resized_data) return;

    memcpy(resized_data + buffer->length, appendix, length);

    buffer->data = resized_data;
    buffer->length += length;
}

void fe_free_buffer(buffer *buffer)
{
    free(buffer->data);
}
