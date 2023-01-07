#include <stdlib.h>
#include <string.h>
#include <helper.h>

#include <highlight.h>
#include <buffer.h>


static char *get_file_extension( const char *filename ) {
    Buffer *buf = fe_create_buffer();

    for( int i = 0; filename[i] != '\0'; i++ )
    {
        char c = filename[i];

        if (c == '.')
        {
            fe_free_buffer(buf);
            buf = fe_create_buffer();
        } 
        else 
        {
            fe_append_to_buffer(buf, &c, 1);
        }
    }

    char nil = '\0';
    fe_append_to_buffer( buf, &nil, 1 );
    
    return buf->data;
}


static int str_empty( char *str ) {
    return str[0] == '\0';
}

static Buffer *apply_color( Highlighter *h, Buffer *buf_text ) {
    char *str_text = malloc(sizeof( char ) * ( buf_text->length + 1 ));
    memcpy(str_text, buf_text->data, buf_text->length);
    str_text[buf_text->length] = '\0';

    Buffer *new_text = fe_create_buffer();

    for( int i = 0; i < h->expressions_len; i++ ) 
    {
        HighlightExpression he = h->expressions[i];
        int err = regexec( &he.expression, str_text, 0, NULL, 0 );

        if ( ! err )
        {
            char *ansii_end = "\E[0m";
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
    Highlighter *h = (Highlighter*) malloc( sizeof( Highlighter ));
    h->filetype = get_file_extension( filename );

    if( strcmp(h->filetype, "c") || strcmp(h->filetype, "h") ) {
        h->expressions = malloc( sizeof( HighlightExpression ) * 5 );
        h->expressions[0] = *fe_init_highlight_expression( "^(void|struct|int|char|return|if|for|while|do|else|const)$", "\E[0;31m" );
        h->expressions[1] = *fe_init_highlight_expression( "^[0-9]{1,}$", "\E[0;34m" );
        h->expressions[2] = *fe_init_highlight_expression( "^[a-zA-Z0-9]{1,}\\(", "\E[0;35m" );
        h->expressions[3] = *fe_init_highlight_expression( "^[A-Z][a-zA-Z]*$", "\E[0;33m" );
        h->expressions[4] = *fe_init_highlight_expression( "^(\\+)$", "\E[0;33m" );
        h->expressions_len = 5;
    } else {
        h->expressions_len = 0;
    }

    return h;
}

Buffer *fe_highlight(Highlighter *h, Buffer *text) {
    if ( h == NULL || h->expressions_len == 0 )
    {
        return text;
    }

    Buffer *new_text = fe_create_buffer();
    Buffer *buf = fe_create_buffer();

    for( int i = 0; i <= text->length; i++ ) {
        char c = i < text->length ? text->data[i] : ' ';
        
        if( (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '(' || c == '_' )
        {
            fe_append_to_buffer( buf, &c, 1 );
        } 
        else 
        {
            if ( buf->data != NULL && !str_empty( buf->data ) ) 
            {
                Buffer *val = apply_color( h, buf );
                fe_append_to_buffer( new_text, val->data, val->length );
                fe_free_buffer(val);
                fe_free_buffer(buf);
                buf = fe_create_buffer();
            }

            fe_append_to_buffer( new_text, &c, 1 );
        }
    }

    fe_free_buffer(buf);
    return new_text;
}

void fe_free_highlighter(Highlighter *h) {
    free( h->filetype );
    free( h->expressions );
    free( h );
}
