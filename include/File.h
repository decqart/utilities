#ifndef FILE_H
#define FILE_H

#include <stddef.h>

char *read_file(const char *file_path, size_t *file_size);
void write_file(const char *file_name, char *buffer, size_t size);

#endif /* FILE_H */

#ifdef FILE_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>

char *read_file(const char *file_path, size_t *file_size)
{
    FILE *file = fopen(file_path, "r");
    if (file == NULL) return NULL;

    fseek(file, 0, SEEK_END);

    long size = ftell(file);
    if (size < 0) goto error;
    if (file_size != NULL)
        *file_size = size;

    char *buffer = malloc(size+1);
    if (buffer == NULL) goto error;

    fseek(file, 0, SEEK_SET);

    fread(buffer, 1, size, file);

    buffer[size] = '\0';

    fclose(file);
    return buffer;
error:
    fclose(file);
    return NULL;
}

void write_file(const char *file_name, char *buffer, size_t size)
{
    FILE *file = fopen(file_name, "w");
    fwrite(buffer, 1, size, file);
    fclose(file);
}

#endif /* FILE_IMPLEMENTATION */
