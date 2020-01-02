#ifndef FEMTO_H
#define FEMTO_H

#include <stdlib.h>
#include <stdio.h>
#include <termios.h>

size_t fe_read_file(char *filename, char **content);
void fe_write_file(char *filename, char *content, size_t length);

void fe_enable_raw_mode();
void fe_disable_raw_mode();
#endif // FEMTO_H
