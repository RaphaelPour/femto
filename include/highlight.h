#ifndef HIGHLIGHT_H
#define HIGHLIGHT_H

#include <regex.h>

#include <buffer.h>

typedef struct {
    /* the regular expression to match this expression */
    regex_t expression;

    /* the color to highlight this expression in */
    char *color;
} HighlightExpression;

typedef struct {
    /* type of the file / file extension */
    char *filetype;

    /* array of expressions to colorize */
    HighlightExpression *expressions;

    /* length of expressions array */
    int expressions_len;
} Highlighter;

Highlighter *fe_init_highlighter( const char *filename );
Buffer *fe_highlight( Highlighter *h, Buffer *text );
void fe_free_highlighter(Highlighter *h);
#endif