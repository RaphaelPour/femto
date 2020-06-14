#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <helper.h>
#include <fileio.h>

bool fe_file_load( Session *s )
{
    FILE *file_handle = fopen( s->filename, "r" );
    size_t read_bytes = 0;
    char chunk[1024];
    size_t line_index = 0;

    // TODO: If file doesn't exist, just return zero. This ensures that
    //       new files can also be created. Ensure that the caller can
    //       handle this (content will still be a null pointer)
    if( file_handle == NULL )
    {
        perror( "open file for read" );
        return false;
    }

    // Read lines until EOF
    while( ! feof( file_handle ))
    {
        // TODO: With this implementation, lines greater than the chunk size
        //       will be split into multiple lines.
        if( fgets( (char*)&chunk, sizeof(chunk), file_handle ) == NULL )
        {
            if( feof( file_handle ))
            {
                //printf("EOF reached\n");
                break;
            }
            perror( "read file" );
            exit( EXIT_FAILURE );
        }

        // Get rid of the new line
        read_bytes = strlen( chunk );

        // Drop last char if it is a new line
        if( chunk[read_bytes-1] == '\n' )
            read_bytes--;

        //lprintf(LOG_INFO, "R [%02zu] (%zu): %.*s", line_index+1, read_bytes, read_bytes, chunk);

        // Realloc here since we need to fill the lines 
        s->lines = (Line*) realloc( s->lines,sizeof(Line) * ( line_index+1 ));

        s->lines[ line_index ].index = line_index;
        s->lines[ line_index ].length = read_bytes;
        s->lines[ line_index ].content = (char*) malloc( read_bytes );

        memcpy( s->lines[ line_index ].content, &chunk, read_bytes );

        s->content_length += read_bytes;
        
        //lprintf(LOG_INFO, "L [%02zu] (%zu): %.*s", s->lines[line_index].index, s->lines[line_index].length,
        //    s->lines[line_index].length, s->lines[line_index].content);
        
        line_index++;
    }

    /* Even if the file is empty, the editor needs at least one line */
    if( line_index == 0 )
    {
        s->lines = (Line*) malloc( sizeof(Line) );
        s->lines[0].index = 0;
        s->lines[0].length = 0;
        s->lines[0].content = (char*) malloc( 0 );

        line_index++;
    }



    s->line_count = line_index;

    /* Clear dirty bit */
    s->dirty = false;

    // Finish file operation by closing the file resource
    fclose( file_handle );

    return true;
}

bool fe_file_save( Session *s )
{
    FILE *file_handle = fopen( s->filename, "w" );
    
    if( ! file_handle )
    {
        lprintf( LOG_ERROR, "Error opening file %s for saving", s->filename );
        return false;
    }

    /* 
     * Don't write anything if the content includes only one empty line.
     * Just touch the file and close it.
     *
     * Two lines are a valid 
     */
    if( s->line_count == 1 && s->lines[ 0 ].length == 0 )
    {
        /* Clear dirty bit */
        s->dirty = false;

        fclose( file_handle );
        return true;
    }

    /* Iterate over all lines and write content with trailing new line to file */
    for( int i = 0; i < s->line_count; i++)
    {
        Line *line = &s->lines[ i ];
        
        size_t bytes_written = fwrite( line->content, 1, line->length, file_handle );

        if( bytes_written < line->length )
            lprintf( LOG_WARNING, 
                     "Wrote less bytes to file than expected. Line has %d, wrote %d bytes",
                     line->length,
                     bytes_written );

        /*
         * Don't write a new line for the last line since new lines
         * _join_ two lines.
         */
        //if(i == s->line_count - 1) continue;
        char newLine = '\n';
        bytes_written = fwrite( &newLine, 1, 1, file_handle );
        
        if( bytes_written != sizeof(newLine) )
            lprintf( LOG_WARNING,
                     "Worte less bytes to file than expected. New line has %d, wrote %d bytes",
                     sizeof(newLine),
                     bytes_written );
                     
    }

    fclose( file_handle );


    /* Clear dirty bit */
    s->dirty = false;

    return true;
}
