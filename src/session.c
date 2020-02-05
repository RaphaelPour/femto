#include "session.h"

session* fe_init_session()
{
    session *s = (session*) malloc(sizeof(session));
    
    s->filename = NULL;
    s->size = (terminal_size){{0},{0}};
    s->position = (position){{0},{0}};

    return s;
}

void fe_free_session(session *s)
{
    free(s);
}
