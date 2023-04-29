#ifndef FILE_H
#define FILE_H

#include <Str.h>

Str read_file(const char *file_path);
void write_file(const char *file_name, Str buffer);

#endif /* FILE_H */

#ifdef FILE_IMPLEMENTATION

Str read_file(const char *file_path)
{
    FILE *file = fopen(file_path, "r");
    if (file == NULL) return NULLSTR;

    fseek(file, 0, SEEK_END);

    long size = ftell(file);
    if (size < 0) goto error;

    char *buffer = malloc(size+1);
    if (buffer == NULL) goto error;

    fseek(file, 0, SEEK_SET);

    fread(buffer, 1, size, file);

    buffer[size] = '\0';

    fclose(file);
    return (Str) { buffer, size };
error:
    fclose(file);
    return NULLSTR;
}

void write_file(const char *file_name, Str buffer)
{
    FILE *file = fopen(file_name, "w");
    fwrite(buffer.str, 1, buffer.len, file);
    fclose(file);
}

#endif /* FILE_IMPLEMENTATION */
