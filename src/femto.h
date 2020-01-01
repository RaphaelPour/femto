#ifndef FEMTO_H
#define FEMTO_H

#include <stdlib.h>
#include <stdio.h>

size_t fe_read_file(char *filename, char **content);
void fe_write_file(char *filename, char *content, size_t length);

#endif // FEMTO_H
