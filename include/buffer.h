#ifndef BUFFER_H
#define BUFFER_H

// Screen buffer
typedef struct
{
    char *data;
    unsigned length;
}Buffer;

Buffer* fe_create_buffer();
void fe_append_to_buffer(Buffer *buffer, const char *appendix, unsigned length);
char *fe_buf_to_str(Buffer *buffer);
void fe_free_buffer(Buffer *buffer);
#endif // BUFFER_H
