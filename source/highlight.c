#include <stdlib.h>
#include <string.h>
#include <helper.h>

#include <highlight.h>
#include <buffer.h>
#include <screen.h>

static int char_between(char c, char min, char max)
{
    return c >= min && c <= max;
}

static char* get_file_extension( const char *filename ) {
    char* ext = strrchr(filename, '.');
    // If filename has an extension (ext is not NULL) remove extension dot (ext + 1)
    return ext ? ext + 1 : NULL;
}

static Buffer *apply_color( Highlighter *h, Buffer *buf_text ) {
    char *str = fe_buf_to_str(buf_text);
    Buffer *new_text = fe_create_buffer();

    // Iterate over every registered expression
    for( int i = 0; i < h->expressions_len; i++ ) 
    {
        HighlightExpression he = h->expressions[i];
        // Check whether expression matches the token
        int err = regexec( &he.expression, str, 0, NULL, 0 );

        if ( ! err )
        {
            char *ansii_end = RESET_COLOR;
            int ansii_end_len = 5;

            fe_append_to_buffer( new_text, he.color, strlen(he.color) ); 
            fe_append_to_buffer( new_text, buf_text->data, buf_text->length );
            fe_append_to_buffer( new_text, ansii_end, ansii_end_len );

            return new_text;
        }
        else if ( err != REG_NOMATCH )
        {
            char msgbuff[100];
            regerror(err, &he.expression, msgbuff, 100);
            lprintf( LOG_ERROR, msgbuff );
        }
    }

    fe_append_to_buffer( new_text, buf_text->data, buf_text->length );
    return new_text;
}

static HighlightExpression *fe_init_highlight_expression( char *str_expr, char *color ) {
    HighlightExpression *he = (HighlightExpression*) malloc( sizeof( HighlightExpression ));

    regcomp( &he->expression, str_expr, REG_EXTENDED );
    he->color = color;

    return he;
}


Highlighter *fe_init_highlighter( const char *filename ) {
    if ( ! filename ) return NULL;

    Highlighter *h = (Highlighter*) malloc( sizeof( Highlighter ) );
    h->filetype = get_file_extension( filename );
    if ( ! h->filetype )
    {
        h->filetype = "";
    }

    // C syntax highlighting
    if ( strcmp(h->filetype, "c") == 0 || strcmp(h->filetype, "h") == 0 ) {
        h->expressions_len = 7;

        // If there are overlapping expressions, the one that is higher is prioritized
        h->expressions = malloc( sizeof( HighlightExpression ) * h->expressions_len );
        // keywords
        h->expressions[0] = *fe_init_highlight_expression( "^(void|struct|int|char|return|if|for|while|do|else|const)$", RED_COLOR );
        // numbers
        h->expressions[1] = *fe_init_highlight_expression( "^[0-9]+$", BLUE_COLOR );
        // hexadecimal
        h->expressions[2] = *fe_init_highlight_expression( "^0x[0-9a-fA-F]+$", BLUE_COLOR );
        // binary
        h->expressions[3] = *fe_init_highlight_expression( "^0b[01]+$", BLUE_COLOR );
        // functions
        h->expressions[4] = *fe_init_highlight_expression( "^[a-zA-Z0-9]{1,}\\(", PURPLE_COLOR );
        // "classes" / structs
        h->expressions[5] = *fe_init_highlight_expression( "^[A-Z][a-zA-Z]*$", YELLOW_COLOR );
        // strings
        h->expressions[6] = *fe_init_highlight_expression( "^\"(.*)?[^(\\\")]\"$", YELLOW_COLOR );
    }
    // No language detected
    else {
        h->expressions_len = 0;
        h->expressions = NULL;
    }

    return h;
}

Buffer *fe_highlight( Highlighter *h, Buffer *text ) {
    if ( ! h || h->expressions_len == 0 )
    {
        return text;
    }

    // Buffer for the rendered text
    Buffer *new_text = fe_create_buffer();
    // Buffer for the lexed sequences
    Buffer *buf = fe_create_buffer();

    // Iterate over every character in the text
    for( int i = 0; i <= text->length; i++ ) {
        // Set c to the current character or ' ' for one additional cycle
        char c = i < text->length ? text->data[i] : ' ';
        
        // Check if the currently lexed chars represent a string
        if( (buf->length > 0 && buf->data[0] == '"') || (buf->length == 0 && c == '"') )
        {
            fe_append_to_buffer( buf, &c, 1 );
            // Check whether the string was terminated with an unescaped double quote
            if( buf->length > 1 && c == '"' && buf->data[buf->length - 2] != '\\' )
            {
                Buffer *val = apply_color( h, buf );
                fe_append_to_buffer( new_text, val->data, val->length );
                fe_free_buffer( val );
                fe_free_buffer( buf );
                buf = fe_create_buffer();
            }
        }
        // Check if character is non-terminating
        else if( ( char_between(c, 'a', 'z') || 
                char_between(c, 'A', 'Z') || 
                char_between(c, '0', '9') || 
                c == '_' || c == '(' ) &&
                ( buf->length == 0 || (buf->length > 0 && buf->data[buf->length - 1] != '(') ) 
                )
        {
            fe_append_to_buffer( buf, &c, 1 );
        }
        else
        {
            if ( buf->length > 0 ) 
            {
                Buffer *val = apply_color( h, buf );
                fe_append_to_buffer( new_text, val->data, val->length );
                fe_free_buffer(val);

                // Reset buffer
                fe_free_buffer(buf);
                buf = fe_create_buffer();
            }

            fe_append_to_buffer( new_text, &c, 1 );
        }
    }

    if ( buf->length > 0 )
    {
        fe_append_to_buffer( new_text, buf->data, buf->length );
    }

    fe_free_buffer(buf);
    return new_text;
}

void fe_free_highlighter(Highlighter *h) {
    if ( ! h ) return;

    free( h->expressions );
    free( h );
}
