#ifndef FILE_IO_H
#define FILE_IO_H

#include <stdbool.h>
#include <session.h>

bool fe_file_load( Session *s );
bool fe_file_save( Session *s );

#endif // FILE_IO_H
