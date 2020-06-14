#include <stdlib.h>
#include <ctype.h>

#include "ui.h"

static char* fe_user_prompt( Session *s, char* prompt );

bool fe_safe_file_dialog( Session *s )
{
    if( ! s->filename )
    {
        char * filename = fe_user_prompt( s, "Filename:" );
        if( ! filename ){
            free( filename );
            return false;
        }

        fe_set_filename( s, filename );
        free( filename );
    }

    if( ! fe_file_save( s ))
    {
        lprintf(LOG_ERROR, "Error saving file");
        return false;
    }

    return true;
}

bool fe_quit_dialog( Session *s )
{
    if( ! s->dirty ) return true;

    fe_refresh_screen( s, fe_generate_prompt_status_bar( s,  "Unsaved changes! [D]iscard,[S]ave or [A]bort?", "" ));

    char choice = tolower(fe_get_user_input());

    switch( choice )
    {
    case 'd': return true;
    case 's': return fe_safe_file_dialog( s );
    case 'a':
    default: return false;
    }
}

static char* fe_user_prompt( Session *s, char* prompt )
{
    char c = 0;
    char *answer = (char*) malloc(1);
    answer[0] = '\0';
    int length = 1;

    do{
        fe_refresh_screen( s, fe_generate_prompt_status_bar( s, prompt, answer ) );

        c = fe_get_user_input();

        switch( c )
        {
        case ESCAPE: return NULL;
        default:
            if( isprint( c )){
                answer = (char*) realloc(answer, length + 1);
                answer[length-1] = c;
                answer[length] = '\0';
                length++;
            }
        }


    }while( ! (c == ENTER_MAC || c == ENTER ));

    return answer;
}
