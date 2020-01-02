#ifndef FILE_IO_H
#define FILE_IO_H

size_t fe_file_load(char *filename, char **content);
void fe_file_save(char *filename, char *content, size_t length);

#endif // FILE_IO_H
