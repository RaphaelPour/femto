#include "session.h"

Session* fe_init_session()
{
    Session *s = (Session*) malloc(sizeof(Session));
    
    s->filename = NULL;
    s->size = (TerminalSize){{0},{0}};
    s->position = (TerminalPosition){{0},{0}};

    return s;
}

void fe_free_session(Session *s)
{
    free(s);
}
