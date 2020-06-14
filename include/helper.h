#ifndef HELPER_H
#define HELPER_H

#define MIN( x,y ) ( x<y ? x : y )
#define MAX( x,y ) ( x>y ? x : y )
#define CLAMP( x,l,h ) (( h>l ) ? ( MAX( MIN( x,h ), l )) : ( MAX( MIN( x, l ), h )))

/* Fun-Fact: This directive is a really bad an imprecise log10 */
#define DIGITS(x) ( x <   10 ? 1 :   \
                  ( x <  100 ? 2 :   \
                  ( x < 1000 ? 3 : 4 )))

#define lprintf( l, ... ) ( lprintf_( l, __FILE__, __LINE__, __VA_ARGS__ ));

typedef enum{
    /* Program must exit */
    LOG_ERROR = 0,

    /* Program can still run in defined state */
    LOG_WARNING,

    /* Wow, I didn't know that. Sounds interesting */
    LOG_INFO,

    /* Developers daily bread */
    LOG_DEBUG
} LOG_LEVEL;
void lopen( const char* filename, const LOG_LEVEL level );
void lprintf_( const LOG_LEVEL l, const char* location, const int line, const char *format, ... );
void lclose();
#endif // HELPER_H
