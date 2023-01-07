#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <buffer.h>
#include <helper.h>
#include <fileio.h>
#include <session.h>

static int get_current_index( Session *s );
static Line* get_current_line( Session *s );

Session* fe_init_session( char* filename )
{
    Session *s = (Session*) malloc( sizeof( Session ));

    /* Initialize all content/file specific variables with zero*/
    s->filename = NULL;
    s->cursor_position = (TerminalPosition){{1},{1}};
    s->offset = (TerminalPosition){{0},{0}};
    s->highlighter = NULL;
    s->lines = NULL;
    s->content_length = 0;
    s->line_count = 0;
    s->dirty = false;


    /* Determine terminal properties */
    s->terminal_size = fe_terminal_size();

    /* Set filename independently if it exist or not */
    if( filename )
    {
        fe_set_filename( s, filename );
    }
    
    /* Only load the given file if its exists */
    if( filename && access( filename, F_OK ) != -1 )
    {
        fe_file_load( s );
    }
    else
    {
        /* Prepare session for a new file from scratch */

        lprintf( LOG_DEBUG, "New file" );
        Line *l = (Line*) malloc( sizeof( Line ));

        l->length = 0;
        l->data = NULL;
        
        s->lines = l;
        s->line_count++;
    }

    return s;
}

void fe_set_filename( Session *s, const char *filename )
{
    /* Duplicate filename so we can definetly free it without a doubt. */
    s->filename = (char*) malloc( strlen( filename ) + 1 );
    strcpy( s->filename , filename );
    s->filename[ strlen( filename ) ] = '\0';
    
    s->highlighter = fe_init_highlighter(filename);
}

void fe_dump_session( Session *s )
{
    lprintf( LOG_DEBUG, "----------- Session -----------");
    lprintf( LOG_DEBUG, "Filename       : %s", s->filename );
    lprintf( LOG_DEBUG, "Cursor         : %d/%d", s->cursor_position.x, s->cursor_position.y );
    lprintf( LOG_DEBUG, "Offset         : %d/%d", s->offset.x, s->offset.y );
    lprintf( LOG_DEBUG, "Content length : %d", s->content_length );
    lprintf( LOG_DEBUG, "Line count     : %d", s->line_count );
    lprintf( LOG_DEBUG, "Terminal size  : %d/%d", s->terminal_size.width, s->terminal_size.height-1 );
}

static Line* get_current_line( Session *s )
{
    lprintf( LOG_DEBUG, "Requesting line: cursor.y=%d, offset.y=%d, index=%d",
            s->cursor_position.y,
            s->offset.y,
            get_current_index( s ));

    return &s->lines[ get_current_index( s ) ];
}

static int get_current_index( Session *s)
{
    return s->cursor_position.y - 1 + s->offset.y;
}

static void fe_insert_empty_line( Session *s )
{
    int line_index = get_current_index( s );
    
    /* Add another line */
    s->lines = (Line*) realloc( s->lines, sizeof(Line) * ( s->line_count + 1 ));

    if( ! s->lines )
    {
        lprintf( LOG_ERROR, "Error reallocating memory for a new line at %d\n", line_index );
        return;
    }
    
    /* Move lines if the cursor isn't at the last line */
    if( line_index < s->line_count )
    {
        lprintf( LOG_DEBUG, "Moving %d lines from %d to %d\n",
                s->line_count-line_index,
                line_index,
                line_index + 1 );

        /* Move <line_count> lines from <src> to <dst> and overstep one line */
        Line *src = s->lines + line_index + 1;
        Line *dst = s->lines + line_index + 2;
        int line_count = s->line_count - line_index - 1;
        if( ! memmove( dst, src, sizeof(Line) * line_count))
        {
            lprintf( LOG_ERROR, "Error moving lines" );
            return;
        }
    }
    
    int new_line_index = line_index + 1;
    
    /* Initialize line */
    s->lines[new_line_index].data = NULL;
    s->lines[new_line_index].length = 0;

    /* Update line count */
    s->line_count++;
}


void fe_insert_line( Session *s )
{
    int x = s->cursor_position.x-1;

    fe_insert_empty_line( s );
    
    Line *old_line = get_current_line( s );
    
    /* If the cursor is already at the last position inserting a new empty line is enough */
    lprintf( LOG_DEBUG, "insert new line: x=%d, lineLength=%d\n", x, old_line->length );

    if( x < old_line->length )
    {
        /* Handle what happens when the cursor is in the middle of a line */
        Line *new_line = &s->lines[ get_current_index( s ) + 1 ];

        int length = old_line->length - x;

        lprintf( LOG_DEBUG, "Breaking line at position %d with %d chars. Old line length: %d",x, length, old_line->length );
        /* Copy old line from the cursor to the end to the new line */
        new_line->data = (char*) realloc( new_line->data, length );
        memcpy( new_line->data, old_line->data + x, length );
        new_line->length = length;

        /* Truncate the old line beginning at the cursor position */
        old_line->data = (char*) realloc( old_line->data, old_line->length - length );
        old_line->length -= length;

    }

    /* Set dirty bit */
    s->dirty = true;

    /* Correct cursor */

    /* "Cariage return" */
    s->cursor_position.x = 1;
    s->offset.x = 0;

    fe_move( s, 0, 1 );
    return;

}

void fe_insert_char( Session *s, char c )
{
    int x = s->cursor_position.x-1;
    Line *line = get_current_line( s );

    /* Extend buffer by one line */
    line->data = (char*) realloc( line->data, line->length+1 );

    if( ! line->data )
    {
        lprintf( LOG_ERROR, "Error reallocating memory for new character '%c' in line %d\n", 
                c, get_current_index( s ));
        return;
    }
    
    /* Move memory if the cursor isn't at the last position */
    if( x < line->length ){
        lprintf( LOG_DEBUG, "Moving %d chars of %*.s from %d to %d\n",
                line->length-x, line->length, line->data, x, x+1 );

        char *src = line->data + x;
        char *dst = src + 1;
        if( ! memmove( dst, src, line->length-x ))
        {
            lprintf( LOG_ERROR, "Error moving memory for new character '%c' in line %d", 
                   c, get_current_index( s ));
            return;
        }

    }

    /* Update line length */
    line->length++;

    /* Update file data length */
    s->content_length++;

    /* Finally: Set the new character */
    line->data[x] = c;

    /* Set dirty bit */
    s->dirty = true;

    /* Correct cursor position */
    fe_move( s, 1, 0);
    

    lprintf( LOG_DEBUG, "Inserting '%c' at pos %d. Line length increased from %d to %d", 
            c, x, line->length-1, line->length );
}

static void fe_remove_line( Session *s )
{
    int line_index = get_current_index( s );
    
    /* We need at least two lines to remove one */
    if( line_index == 0 ) return;

    /* 
     * We need the original line length later to fix the cursor
     * while we want the cursor to be at the joint.
     */
    int original_length;

    /* Limit scope of old/new_line */
    {
        Line *old_line = get_current_line( s );
        Line *new_line = &s->lines[ line_index-1 ];

        original_length = new_line->length;
        /* 
         * Check if the line to remove has still content. In this case
         * the content has to be appended to the line above. 
         */
        if( old_line->length > 0 )
        {

            int length = old_line->length;
        
            /* Extend line where the content should be moved to */
            new_line->data = (char*) realloc( 
                    new_line->data, 
                    new_line->length + length 
            );

            /* Append data of the old line to the new one and update the length*/
            memcpy( new_line->data + new_line->length, old_line->data, length );
            new_line->length += length;
        }

        /* Free data of the line which should be removed */
        if( old_line->length > 0 )
            free( old_line->data );
    }

    /* 
     * Move all lines from the cursor+1 one up 
     * except the cursor is at the last line
     */
    if( line_index < s->line_count )
    {
        Line *dst = s->lines + line_index;
        Line *src = dst + 1;
        int lines_to_move = s->line_count - line_index;
        if( ! memmove( dst, src,  sizeof(Line) * lines_to_move ))
        {
            lprintf( LOG_ERROR, "Error moving lines for line remove." );
            return;
        }
    }

    /* 
     * Remove the last line by simply reallocating the lines and decrease the
     * line count by one
     */
    s->lines = (Line*) realloc( s->lines, sizeof(Line) * s->line_count-1 );
    s->line_count--;

    /* 
     * Fix cursor 
     */

    /* Move one line up */
    fe_move( s, 0, -1 );

    /* Move to the joint of both lines */
    if( original_length > 0 )
        fe_move( s, original_length, 0 );
}

void fe_remove_char_after_cursor( Session *s )
{
    int x = s->cursor_position.x -1;
    Line *line = get_current_line( s );
    
    lprintf( LOG_DEBUG, "Remove char after cursor: x=%d, lineLnegth=%d",
            x, line->length-1 );

    /* Do nothing if cursor is at the end of the line */
    if( x == line->length )
    {
        if( s->cursor_position.y == s->line_count-2 )return;

        /*
         * Joining the current with the line below is like
         * joining the current with the line above from another perspective
         */
        fe_move( s, -x, 0 );
        fe_move( s, 0, 1 );
        fe_remove_line( s );
        return;
    }

    /* Move memory if cursor isn't at the last position */
    if(x < line->length-1)
    {
        if( ! memmove(line->data+x, line->data+x+1, line->length-x-1))
        {
            lprintf(LOG_ERROR, "Error moving memory  in line %d", 
                 get_current_index( s ));
            return;
        }
    }

    /* Shrink buffer by reallocating the lines memory */
    line->data = (char*) realloc( line->data, line->length - 1 );

    if( line->length - 1 == 0 ) line->data = NULL;

    /* Update line length */
    line->length--;

    /* Update file data length */
    s->content_length--;
 
    /* Set dirty bit */
    s->dirty = true;
    
    lprintf( LOG_DEBUG, "Shrinking line from %d to %d at pos %d", 
             line->length-1, line->length, x );
}

void fe_remove_char_at_cursor( Session *s )
{
    /* 
     * First -1 to correct the index. Position is 1-based and line representation
     * 0-based.
     * Second -1 because when we say "delete at cursor" we actually mean
     * the char _prior_ the cursor.
     */
    int x = s->cursor_position.x -1 -1;

    if( x < 0 )
    {
      fe_remove_line( s );
      return;
    }

    Line *line = get_current_line( s );

    /* Move memory if cursor isn't at the last position */
    if( x < line->length-1 )
    {
        if( ! memmove( line->data+x, line->data+x+1, line->length-x-1 ))
        {
            lprintf( LOG_ERROR, "Error moving memory in line %d", 
                 get_current_index( s ));
            return;
        }
    }

    /* Shrink buffer by reallocating the lines memory */
    line->data = (char*) realloc( line->data, line->length - 1 );

    if( line->length - 1 == 0 ) line->data = NULL;

    /* Update line length */
    line->length--;

    /* Update file data length */
    s->content_length--;
 
    /* Set dirty bit */
    s->dirty = true;

    /* Fix cursor */
    fe_move( s, -1, 0 );
    
    lprintf( LOG_DEBUG, "Shrinking line from %d to %d at pos %d", 
             line->length-1, line->length, x );
}

static void fe_move_content_offset( Session *s, int x, int y )
{
    lprintf( LOG_INFO, "Before offset: %d/%d", s->offset.x, s->offset.y );
    /* 
     * Move the offset within [0,columns-width-1] and [0,rows-height-1]
     * Otherwise it would be possible to scrool down/right until
     * only one line/col is visible which is useless
     */

    /* Subtract one for the status bar */
    if( y )
        s->offset.y = CLAMP( s->offset.y + y,
                0,
                MAX( s->line_count, s->terminal_size.height-1 ));

    
    /* TODO: Add max col length to session struct to handle it like x */ 
    if( x )
        s->offset.x = CLAMP( s->offset.x + x,
                0,
                s->terminal_size.width-1 );
    lprintf( LOG_INFO, "After offset: %d/%d", s->offset.x, s->offset.y );
}

static void fe_move_cursor( Session *s, int x, int y )
{
    lprintf( LOG_INFO, "Before cursor: %d/%d", s->cursor_position.x, s->cursor_position.y );
    /*
     * Move the cursor within [1,min(width, line_len+1] and [1,min(height,
     * line_count]
     * The user shouldn't scroll out of the buffer.
     */

    int input = s->cursor_position.y + y;
    int low = 1;
    /* Subtract one for the status bar */
    int high = MAX( 1, MIN( s->terminal_size.height-1, s->line_count - s->offset.y ));
    s->cursor_position.y = CLAMP( input, low, high );
    lprintf( LOG_INFO, "CLAMP(input=%d, low=%d, high=%d) = %d", input, low, high, s->cursor_position.y );


    input = s->cursor_position.x + x;
    low = 1;
    int line_index = get_current_index( s );
    
    if( line_index < 0 )
    {
        lprintf( LOG_INFO, "Line index is negative." );
        return;
    }

    if( line_index >= s->line_count )
    {
        lprintf( LOG_INFO, "Line index is greater than line count" );
        return;
    }

    Line line = s->lines[ line_index ];
    int line_length = line.length;

    lprintf( LOG_INFO, "Terminal width: %d", s->terminal_size.width );
    high = MAX( 1,MIN( s->terminal_size.width, line_length+1 ));

    s->cursor_position.x = CLAMP( input, low, high );
    lprintf( LOG_INFO, "CLAMP(input=%d, low=%d, high=%d) = %d", input, low, high, s->cursor_position.x );

    lprintf( LOG_INFO, "After cursor: %d/%d", s->cursor_position.x, s->cursor_position.y );
}


static int fe_end_of_buffer_reached( Session *s, int x, int y )
{
    /*
     * Check if the cursor would move out of the buffer
     */

    /* Leftward movement lead to negative column? */
    if( x < 0 && s->cursor_position.x <= 1  && s->offset.x == 0 )
    {
        lprintf( LOG_DEBUG, "EOB: direction=LEFT, x to low" );
        return 1;
    }

    /* 
     * Rightward movement lead to column buffer out-of-bounds?
     *
     * Add one to the line length in order to allow the cursor be placed
     * behind the last char. This allows to append input.
     */
   if( x >  0 && s->cursor_position.x >= get_current_line(s)->length + 1 )
   {
        lprintf( LOG_DEBUG, "EOB: direction=RIGHT, x to high" );
        return 1;
   }

   /* Upward movement lead to negative row? */
   if( y < 0 && s->cursor_position.y <= 1 && s->offset.y == 0 )
   {
        lprintf( LOG_DEBUG, "EOB: direction=UP, y to low" );
        return 1;
   }

   /* Downward movement lead to row buffer out-of-bounds?*/
   if( y > 0 && s->cursor_position.y >= s->line_count - s->offset.y )
   {
        lprintf( LOG_DEBUG, "EOB: direction=DOWN, y to high" );
        return 1;
   }

   return 0;
}

void fe_move( Session *s, int x, int y )
{
    int offx=0,offy=0,curx=0,cury=0;
    lprintf( LOG_INFO, "Before move: x=%d y=%d, pos: x=%d y=%d, off: x=%d y=%d, size: w=%d h=%d, lines: %d",
            x,y,
            s->cursor_position.x, s->cursor_position.y,
            s->offset.x, s->offset.y,
            s->terminal_size.width, s->terminal_size.height-1,
            s->line_count );

   
    if( fe_end_of_buffer_reached( s, x, y )) 
    {
        lprintf( LOG_INFO, "Enf of buffer. Ignore move." );
        return;
    }
    

    /* 
     * Determine whenever the cursor must be moved or the content
     * must be scrolled
     */
    if( ( x>0 && s->cursor_position.x + x > s->terminal_size.width ) ||
        ( x<0 && s->cursor_position.x + x < 1 ) /* We need + to not neutralize the -1 */
      )
        offx = x;
    else
        curx = x;

    /* Subtract one for the status bar */
    if( ( y>0 && s->cursor_position.y + y > s->terminal_size.height-1 ) ||
        ( y<0 && s->cursor_position.y + y < 1 ) /* We need + to not neutralize the -1 */
      )
        offy = y;
    else
        cury = y;

    lprintf( LOG_INFO, "∆off: x=%d y=%d ∆cur: x=%d y=%d", offx, offy, curx, cury );

    fe_move_cursor( s, curx, cury );
    fe_move_content_offset( s, offx, offy );
    lprintf( LOG_INFO, "After move: x=%d y=%d, pos: x=%d y=%d, off: x=%d y=%d, size: w=%d h=%d, lines: %d",
            x,y,
            s->cursor_position.x, s->cursor_position.y,
            s->offset.x, s->offset.y,
            s->terminal_size.width, s->terminal_size.height-1,
            s->line_count );

}

void fe_move_top( Session *s )
{
    lprintf( LOG_INFO, "Before move: pos: x=%d y=%d, off: x=%d y=%d, size: w=%d h=%d, lines: %d",
            s->cursor_position.x, s->cursor_position.y,
            s->offset.x, s->offset.y,
            s->terminal_size.width, s->terminal_size.height-1,
            s->line_count );
    
    s->cursor_position.x = 1;
    s->cursor_position.y = 1;
    s->offset.x = 0;
    s->offset.y = 0;

    lprintf( LOG_INFO, "After move: pos: x=%d y=%d, off: x=%d y=%d, size: w=%d h=%d, lines: %d",
            s->cursor_position.x, s->cursor_position.y,
            s->offset.x, s->offset.y,
            s->terminal_size.width, s->terminal_size.height-1,
            s->line_count );
}

void fe_move_bottom( Session *s )
{
    lprintf( LOG_INFO, "Before move: pos: x=%d y=%d, off: x=%d y=%d, size: w=%d h=%d, lines: %d",
            s->cursor_position.x, s->cursor_position.y,
            s->offset.x, s->offset.y,
            s->terminal_size.width, s->terminal_size.height-1,
            s->line_count );
    
    s->cursor_position.x = 0;
    s->cursor_position.y = s->line_count;
    s->offset.x = 0;
    s->offset.y = MAX( s->line_count - (s->terminal_size.height -1), 0 );

    lprintf( LOG_INFO, "After move: pos: x=%d y=%d, off: x=%d y=%d, size: w=%d h=%d, lines: %d",
            s->cursor_position.x, s->cursor_position.y,
            s->offset.x, s->offset.y,
            s->terminal_size.width, s->terminal_size.height-1,
            s->line_count );
}

void fe_move_page_up( Session *s )
{
    int offset = MIN(
            s->terminal_size.height, 
            s->cursor_position.y + s->offset.y);

    /* 
     * Since bound-checking drops move if out-of-bound this has to be
     * made iteratively 
     */
    while(offset--) fe_move( s, 0, -1 );
}

void fe_move_page_down( Session *s )
{
    int offset = MIN( 
            s->terminal_size.height, 
            s->line_count - ( s->cursor_position.y + s->offset.y ));

    fe_move( s, 0, offset );
}


void fe_free_session( Session *s )
{
    free( s->filename );

    // Free lines
    for( int i=0; i < s->line_count; i++ )
    {
        Line *l = &s->lines[i];
        free( l->data );
    }

    free( s->lines );
    fe_free_highlighter( s->highlighter );
    free( s );
}
