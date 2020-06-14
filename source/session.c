#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <buffer.h>
#include <helper.h>
#include <fileio.h>
#include <session.h>

Session* fe_init_session( char* filename )
{
    Session *s = (Session*) malloc( sizeof( Session ));

    /* Initialize all content/file specific variables with zero*/
    s->filename = NULL;
    s->cursor_position = (TerminalPosition){{1},{1}};
    s->offset = (TerminalPosition){{0},{0}};
    s->lines = NULL;
    s->content_length = 0;
    s->line_count = 0;
    s->edit_mode = 0;
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

        l->index = 0;
        l->length = 0;
        l->content = NULL;
        
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
    lprintf( LOG_DEBUG, "Edit mode      : %s", ( s->edit_mode) ? "ON" : "OFF" );
}

void fe_toggle_mode( Session *s )
{
    s->edit_mode = !s->edit_mode;
}

Line* fe_get_current_line( Session *s )
{
    lprintf( LOG_DEBUG, "Requesting line: cursor.y=%d, offset.y=%d, index=%d",
            s->cursor_position.y,
            s->offset.y,
            s->cursor_position.y - 1 + s->offset.y );

    return &s->lines[ s->cursor_position.y -1 + s->offset.y ];
}

static void fe_insert_empty_line( Session *s )
{
    int line_index = s->cursor_position.y + s->offset.y - 1;
    
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

        if( ! memmove( s->lines + line_index + 2, s->lines + line_index + 1, sizeof(Line) * ( s->line_count-line_index - 1 )))
        {
            lprintf( LOG_ERROR, "Error moving lines" );
            return;
        }
    }
    
    int new_line_index = line_index + 1;
    
    /* Initialize line */
    s->lines[new_line_index].index = 0;
    s->lines[new_line_index].content = NULL;
    s->lines[new_line_index].length = 0;

    /* Update line count */
    s->line_count++;
}


void fe_insert_line( Session *s )
{
    int x = s->cursor_position.x-1;

    fe_insert_empty_line( s );
    
    Line *oldLine = fe_get_current_line( s );
    
    /* If the cursor is already at the last position inserting a new empty line is enough */
    lprintf( LOG_DEBUG, "insert new line: x=%d, lineLength=%d\n", x, oldLine->length );

    if( x < oldLine->length )
    {
        /* Handle what happens when the cursor is in the middle of a line */
        Line *newLine = &s->lines[ s->cursor_position.y - 1 + s->offset.y + 1 ];

        int length = oldLine->length - x;

        lprintf( LOG_DEBUG, "Breaking line at position %d with %d chars. Old line length: %d",x, length, oldLine->length );
        /* Copy old line from the cursor to the end to the new line */
        newLine->content = (char*) realloc( newLine->content, length );
        memcpy( newLine->content, oldLine->content + x, length );
        newLine->length = length;

        /* Truncate the old line beginning at the cursor position */
        oldLine->content = (char*) realloc( oldLine->content, oldLine->length - length );
        oldLine->length -= length;

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
    Line *line = fe_get_current_line( s );

    /* Extend buffer by reallocating the lines memory */
    line->content = (char*) realloc( line->content, line->length+1 );

    if( ! line->content )
    {
        lprintf( LOG_ERROR, "Error reallocating memory for new character '%c' in line %d\n", 
                c, s->cursor_position.y -1 + s->offset.y );
        return;
    }
    
    /* Move memory if the cursor isn't at the last position */
    if( x < line->length ){
        lprintf( LOG_DEBUG, "Moving %d chars of %*.s from %d to %d\n",
                line->length-x, line->length, line->content, x, x+1 );
        if( ! memmove( line->content+x+1, line->content+x, line->length-x ))
        {
            lprintf( LOG_ERROR, "Error moving memory for new character '%c' in line %d", 
                   c, s->cursor_position.y -1 + s->offset.y );
            return;
        }

    }

    /* Update line length */
    line->length++;

    /* Update file content length */
    s->content_length++;

    /* Finally: Set the new character */
    line->content[x] = c;

    /* Set dirty bit */
    s->dirty = true;

    /* Correct cursor position */
    fe_move( s, 1, 0);
    

    lprintf( LOG_DEBUG, "Inserting '%c' at pos %d. Line length increased from %d to %d", 
            c, x, line->length-1, line->length );
}

static void fe_remove_line( Session *s )
{
    int line_index = s->cursor_position.y + s->offset.y - 1;
    
    /* We need at least two lines to remove one */
    if( line_index == 0 ) return;

    Line *oldLine = fe_get_current_line( s );
    Line *newLine = &s->lines[ line_index-1 ];

    /* 
     * We need the original line length later to fix the cursor
     * while we want the cursor to be at the joint.
     */
    int originalLength = newLine->length;
    /* 
     * Check if the line to remove has still content. In this case
     * the content has to be appended to the line above. 
     */
    if( oldLine->length > 0 )
    {

        int length = oldLine->length;
    
        /* Extend line where the content should be moved to */
        newLine->content = (char*) realloc( newLine->content, newLine->length + length );

        /* Append content of the old line to the new one and update the length*/
        memcpy( newLine->content + newLine->length, oldLine->content, length );
        newLine->length += length;
    }

    /* Free content of the line which should be removed */
    if( oldLine->length > 0 )
        free( oldLine->content );
    
    /* 
     * Set line to NULL so nobody can do bad things while the linesw
     * get moved and reallocated
     */
    oldLine = NULL;
    newLine = NULL;

    /* 
     * Move all lines from the cursor+1 one up 
     * except the cursor is at the last line
     */
    if( line_index < s->line_count )
    {
        int linesToMove = s->line_count - line_index;
        if( ! memmove( s->lines + line_index, s->lines + line_index + 1, sizeof(Line) * ( linesToMove )))
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
    if( originalLength > 0 )
        fe_move( s, originalLength, 0 );
}

void fe_remove_char_after_cursor( Session *s )
{
    int x = s->cursor_position.x -1;
    Line *line = fe_get_current_line( s );
    
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
        if( ! memmove(line->content+x, line->content+x+1, line->length-x-1))
        {
            lprintf(LOG_ERROR, "Error moving memory  in line %d", 
                 s->cursor_position.y -1 + s->offset.y);
            return;
        }
    }

    /* Shrink buffer by reallocating the lines memory */
    line->content = (char*) realloc( line->content, line->length - 1 );

    if( line->length - 1 == 0 ) line->content = NULL;

    /* Update line length */
    line->length--;

    /* Update file content length */
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

    Line *line = fe_get_current_line( s );

    /* Move memory if cursor isn't at the last position */
    if( x < line->length-1 )
    {
        if( ! memmove( line->content+x, line->content+x+1, line->length-x-1 ))
        {
            lprintf( LOG_ERROR, "Error moving memory  in line %d", 
                 s->cursor_position.y -1 + s->offset.y );
            return;
        }
    }

    /* Shrink buffer by reallocating the lines memory */
    line->content = (char*) realloc( line->content, line->length - 1 );

    if( line->length - 1 == 0 ) line->content = NULL;

    /* Update line length */
    line->length--;

    /* Update file content length */
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
     * Move the offset withing [0,columns-width-1] and [0,rows-height-1]
     * Otherwise it would be possible to scrool down/right until
     * only one line/col is visible which is useless
     */
    
    /* DEBUG: Don't set any offset at all to fix the cursor positioning */
    //return;

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
    int line_index = s->cursor_position.y + s->offset.y - 1;
    
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
   if( x >  0 && s->cursor_position.x >= fe_get_current_line(s)->length + 1 )
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

void fe_free_session( Session *s )
{
    free( s->filename );

    // Free lines
    for( int i=0; i < s->line_count; i++ )
    {
        Line *l = &s->lines[i];
        free( l->content );
    }

    free( s->lines );
    free( s );
}
