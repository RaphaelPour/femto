#ifndef SESSION_H
#define SESSION_H

#include "terminal.h"

typedef struct {
    char *filename;
    size terminal_size;
    position position;
} session

session* fe_init_session();
void fe_free_session(session *s);

#endif // SESSION_H
