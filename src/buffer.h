
#include <stdlib.h>
#include <string.h>

// Screen buffer
typedef struct buffer
{
    char *data;
    unsigned length;
}buffer;

buffer* fe_create_buffer();
void fe_append_to_buffer(buffer *buffer, const char *appendix, unsigned length);
void fe_free_buffer(buffer *buffer);
