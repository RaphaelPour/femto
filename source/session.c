#include "session.h"

Session* fe_init_session(const char* filename)
{
    Session *s = (Session*) malloc(sizeof(Session));

    // Initialize all content/file specific variables with zero
    s->filename = filename;
    s->cursor_position = (TerminalPosition){{1},{1}};
    s->offset = (TerminalPosition){{0},{0}};
    s->lines = NULL;
    s->content_length = 0;
    s->line_count = 0;
    s->edit_mode = 0;

    // Determine terminal properties
    s->terminal_size = fe_terminal_size();

    if(filename)
        fe_file_load(filename, s);

    return s;
}

void fe_toggle_mode(Session *s)
{
    s->edit_mode = !s->edit_mode;
}



static void fe_move_content_offset(Session *s, int x, int y)
{
    /* 
     * Move the offset withing [0,columns-width-1] and [0,rows-height-1]
     * Otherwise it would be possible to scrool down/right until
     * only one line/col is visible which is useless
     */
    
    /* DEBUG: Don't set any offset at all to fix the cursor positioning */
    return;

    if(x)
        s->offset.x = CLAMP(s->offset.x + x,
                0,
                MAX(s->line_count, s->terminal_size.height - s->line_count));

    
    /* TODO: Add max col length to session struct to handle it like x */ 
    if(y)
        s->offset.y = CLAMP(s->offset.y + y,
                0,
                s->terminal_size.width-1);
}

static void fe_move_cursor(Session *s, int x, int y)
{
    /*
     * Move the cursor within [1,min(width, line_len+1] and [1,min(height,
     * line_count]
     * The user shouldn't scroll out of the buffer.
     */

    s->cursor_position.y = 
        CLAMP(s->cursor_position.y + y,
        1,
        MIN(s->terminal_size.height, s->line_count));
    
    s->cursor_position.x = 
        CLAMP(s->cursor_position.x + x,
        1,
        MIN(s->terminal_size.width, s->lines[s->cursor_position.y-1].length+1));
}

void fe_move(Session *s, int x, int y)
{
    int offx=0,offy=0,curx=0,cury=0;
    lprintf(LOG_INFO, "Before mov: %2d/%2d, pos: %2d/%2d, off: %3d/%3d, size: %3d/%3d, lcount: %d",
            x,y,
            s->cursor_position.x, s->cursor_position.y,
            s->offset.x, s->offset.y,
            s->terminal_size.width, s->terminal_size.height,
            s->line_count);
    /* 
     * Determine whenever the cursor must be moved or the content
     * must be scrolled
     */
    if( (x>0 && s->cursor_position.x + x > s->terminal_size.width) ||
        (x<0 && s->cursor_position.x - x < 0) )
        offx = x;
    else
        curx = x;

    if( (y>0 && s->cursor_position.y + y > s->terminal_size.height) ||
        (y<0 && s->cursor_position.y - y < 0) )
        offy = y;
    else
        cury = y;

    fe_move_cursor(s, curx, cury);
    fe_move_content_offset(s, offx, offy);
    
    lprintf(LOG_INFO, "After mov: %2d/%2d, pos: %2d/%2d, off: %3d/%3d, size: %3d/%3d, lcount: %d",
            x,y,
            s->cursor_position.x, s->cursor_position.y,
            s->offset.x, s->offset.y,
            s->terminal_size.width, s->terminal_size.height,
            s->line_count);
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

        lprintf(LOG_INFO, "R [%02zu] (%zu): %.*s", line_index+1, read_bytes, read_bytes, chunk);

        // Realloc here since we need to fill the lines 
        s->lines = (Line*) realloc(s->lines,sizeof(Line) * (line_index+1));

        s->lines[line_index].index = line_index;
        s->lines[line_index].length = read_bytes;
        s->lines[line_index].content = (char*) malloc(read_bytes);

        memcpy(s->lines[line_index].content, &chunk, read_bytes);

        s->content_length += read_bytes;
        
        lprintf(LOG_INFO, "L [%02zu] (%zu): %.*s", s->lines[line_index].index, s->lines[line_index].length,
            s->lines[line_index].length, s->lines[line_index].content);
        
        line_index++;
    }

    s->line_count = line_index;

    // Finish file operation by closing the file resource
    fclose(file_handle);

}

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
