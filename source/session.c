#include "session.h"

Session* fe_init_session(char* filename)
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

void fe_dump_session(Session *s)
{
    lprintf(LOG_DEBUG, "----------- Session -----------");
    lprintf(LOG_DEBUG, "Filename       : %s", s->filename);
    lprintf(LOG_DEBUG, "Cursor         : %d/%d", s->cursor_position.x, s->cursor_position.y);
    lprintf(LOG_DEBUG, "Offset         : %d/%d", s->offset.x, s->offset.y);
    lprintf(LOG_DEBUG, "Content length : %d", s->content_length);
    lprintf(LOG_DEBUG, "Line count     : %d", s->line_count);
    lprintf(LOG_DEBUG, "Terminal size  : %d/%d", s->terminal_size.width, s->terminal_size.height);
    lprintf(LOG_DEBUG, "Edit mode      : %s", (s->edit_mode) ? "ON" : "OFF");
}

void fe_toggle_mode(Session *s)
{
    s->edit_mode = !s->edit_mode;
}



static void fe_move_content_offset(Session *s, int x, int y)
{
    lprintf(LOG_INFO, "Before offset: %d/%d", s->offset.x, s->offset.y);
    /* 
     * Move the offset withing [0,columns-width-1] and [0,rows-height-1]
     * Otherwise it would be possible to scrool down/right until
     * only one line/col is visible which is useless
     */
    
    /* DEBUG: Don't set any offset at all to fix the cursor positioning */
    //return;

    if(y)
        s->offset.y = CLAMP(s->offset.y + y,
                0,
                MAX(s->line_count, s->terminal_size.height));

    
    /* TODO: Add max col length to session struct to handle it like x */ 
    if(x)
        s->offset.x = CLAMP(s->offset.x + x,
                0,
                s->terminal_size.width-1);
    lprintf(LOG_INFO, "After offset: %d/%d", s->offset.x, s->offset.y);
}

static void fe_move_cursor(Session *s, int x, int y)
{
    lprintf(LOG_INFO, "Before cursor: %d/%d", s->cursor_position.x, s->cursor_position.y);
    /*
     * Move the cursor within [1,min(width, line_len+1] and [1,min(height,
     * line_count]
     * The user shouldn't scroll out of the buffer.
     */

    int input = s->cursor_position.y + y;
    int low = 1;
    int high = MAX(1, MIN(s->terminal_size.height, s->line_count - s->offset.y));
    s->cursor_position.y = CLAMP(input, low,high);
    lprintf(LOG_INFO, "CLAMP(input=%d, low=%d, high=%d) = %d", input, low, high, s->cursor_position.y);


    input = s->cursor_position.x + x;
    low = 1;
    int line_index = s->cursor_position.y-1;
    
    if(line_index < 0)
    {
        lprintf(LOG_INFO, "Line index is negative.");
        return;
    }

    if(line_index >= s->line_count)
    {
        lprintf(LOG_INFO, "Line index is greater than line count");
        return;
    }

    Line line = s->lines[line_index];
    int line_length = line.length;

    lprintf(LOG_INFO, "Terminal width: %d", s->terminal_size.width);
    high = MAX(1,MIN(s->terminal_size.width, line_length+1));

    s->cursor_position.x = CLAMP(input, low, high);
    lprintf(LOG_INFO, "CLAMP(input=%d, low=%d, high=%d) = %d", input, low, high, s->cursor_position.x);

    lprintf(LOG_INFO, "After cursor: %d/%d", s->cursor_position.x, s->cursor_position.y);
}

static int fe_end_of_buffer_reached(Session *s, int x, int y)
{
    /*
     * Check if the cursor would move out of the buffer
     */

    return (
            /* Leftward movement lead to negative column? */
            (x<0 && s->cursor_position.x <= 1  && s->offset.x == 0) ||

            /* Rightward movement lead to column buffer out-of-bounds? */
           (x>0 && s->cursor_position.x >= s->lines[s->cursor_position.y-1].length) ||

           /* Upward moevement lead to negative row? */
           (y<0 && s->cursor_position.y <= 1 && s->offset.y == 0) ||

           /* Downward moevement lead to row buffer out-of-bounds?*/
           (y>0 && s->cursor_position.y >= s->line_count - s->offset.y)
           );
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

    if(fe_end_of_buffer_reached(s, x, y)) 
    {
        lprintf(LOG_INFO, "Enf of buffer. Ignore move.");
        return;
    }

    /* 
     * Determine whenever the cursor must be moved or the content
     * must be scrolled
     */
    if( (x>0 && s->cursor_position.x + x > s->terminal_size.width) ||
        (x<0 && s->cursor_position.x + x < 1) /* We need + to not neutralize the -1 */
      )
        offx = x;
    else
        curx = x;

    if( (y>0 && s->cursor_position.y + y > s->terminal_size.height) ||
        (y<0 && s->cursor_position.y + y < 1) /* We need + to not neutralize the -1 */
      )
        offy = y;
    else
        cury = y;

    lprintf(LOG_INFO, "∆off: %d/%d ∆cur: %d/%d\n", offx, offy, curx, cury);

    fe_move_cursor(s, curx, cury);
    fe_move_content_offset(s, offx, offy);
    lprintf(LOG_INFO, "After mov: %2d/%2d, pos: %2d/%2d, off: %3d/%3d, size: %3d/%3d, lcount: %d",
            x,y,
            s->cursor_position.x, s->cursor_position.y,
            s->offset.x, s->offset.y,
            s->terminal_size.width, s->terminal_size.height,
            s->line_count);

}

void fe_file_load(char *filename, Session *s)
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
