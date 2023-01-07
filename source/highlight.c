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

// ! Does not work as intended
static Buffer *apply_color( Highlighter *h, Buffer *buf_text ) {
    char *str_text = malloc(sizeof( char ) * ( buf_text->length + 1 ));
    memcpy(str_text, buf_text->data, buf_text->length);
    str_text[buf_text->length - 1] = '\0';

    Buffer *new_text = fe_create_buffer();

    for( int i = 0; i < h->expressions_len; i++ ) 
    {
        HighlightExpression he = h->expressions[i];
        int err = regexec( &he.expression, str_text, 0, NULL, 0 );

        if ( strcmp(str_text, "void") ) {
            char *ansii_end = "\e[m";
            int ansii_end_len = 6;

            fe_append_to_buffer( new_text, he.color, strlen(he.color) ); 
            fe_append_to_buffer( new_text, buf_text->data, buf_text->length );
            fe_append_to_buffer( new_text, ansii_end, ansii_end_len );

            lprintf(LOG_INFO, "apply_color -> %*.d" , new_text->length, new_text->data);

            return new_text;
        }
    }

    fe_append_to_buffer( new_text, buf_text->data, buf_text->length );
    return new_text;
}

static HighlightExpression *fe_init_highlight_expression( char *str_expr, char *color ) {
    HighlightExpression *he = (HighlightExpression*) malloc( sizeof( HighlightExpression ));

    regex_t comp_expr;
    regcomp( &comp_expr, str_expr, 0 );

    he->expression = comp_expr;
    he->color = color;

    return he;
}


Highlighter *fe_init_highlighter( const char *filename ) {
    Highlighter *h = (Highlighter*) malloc( sizeof( Highlighter ));
    h->filetype = get_file_extension( filename );

    if( strcmp(h->filetype, "c") || strcmp(h->filetype, "h") ) {
        h->expressions = malloc( sizeof( HighlightExpression ) * 5 );
        h->expressions[0] = *fe_init_highlight_expression( "void|static", "\e[0;31" );
        h->expressions_len = 1;
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
        
        if( (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') )
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
