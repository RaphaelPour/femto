#include "session.h"

Session* fe_init_session(const char* filename)
{
    Session *s = (Session*) malloc(sizeof(Session));

    // Initialize all content/file specific variables with zero
    s->filename = filename;
    s->cursor_position = (TerminalPosition){{0},{0}};
    s->lines = NULL;
    s->content_length = 0;
    s->line_count = 0;

    // Determine terminal properties
    s->terminal_size = fe_terminal_size();

    if(filename)
        fe_file_load(filename, s);

    return s;
}

void fe_file_load(const char *filename, Session *s)
{
    FILE *file_handle = fopen(filename, "r");
    size_t read_bytes = 0;
    char chunk[1024];
    size_t line_index = 0;

    // TODO: If file doesn't exist, just return zero. This ensures that
    //       new files can also be created. Ensure that the caller can
    //       handle this (content will still be a null pointer)
    if(file_handle == NULL)
    {
        perror("open file for read");
        exit(EXIT_FAILURE);
    }

    // Read lines until EOF
    while( ! feof(file_handle))
    {
        // TODO: With this implementation, lines greater than the chunk size
        //       will be split into multiple lines.
        if(fgets((char*)&chunk,sizeof(chunk), file_handle) == NULL)
        {
            if(feof(file_handle))
            {
                //printf("EOF reached\n");
                break;
            }
            perror("read file");
            exit(EXIT_FAILURE);
        }

        // Get rid of the new line
        read_bytes = strlen(chunk);

        // Drop last char if it is a new line
        if(chunk[read_bytes-1] == '\n')
            read_bytes--;

        /*
        printf("[%02zu] (%zu): ", line_index+1, read_bytes);
        int i;
        for(i=0;i<read_bytes;i++)
            putchar(chunk[i]);
        putchar('\n');*/

        // Realloc here since we need to fill the lines 
        s->lines = (Line*) realloc(s->lines,sizeof(Line) * (line_index+1));

        s->lines[line_index].index = line_index;
        s->lines[line_index].length = read_bytes;
        s->lines[line_index].content = (char*) malloc(read_bytes);

        memcpy(s->lines[line_index].content, &chunk, read_bytes);

        s->content_length += read_bytes;
        
        /*
        printf("L [%02zu] (%zu): ", s->lines[line_index].index, s->lines[line_index].length);
        
        for(i=0;i<s->lines[line_index].length;i++)
            putchar(s->lines[line_index].content[i]);
        putchar('\n');*/
        
        line_index++;
    }

    s->line_count = line_index;

    // Finish file operation by closing the file resource
    fclose(file_handle);

}


void fe_free_session(Session *s)
{
    // Free lines
    int i;
    for(i=0;i<s->line_count;i++)
    {
        Line *l = &s->lines[i];
        if(l && l->content)
            free(l->content);
    }

    free(s->lines);
}
