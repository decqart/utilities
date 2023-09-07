#ifndef STRINGARRAY_H
#define STRINGARRAY_H

#include <stddef.h>

typedef struct {
    char **data;
    size_t size;
    size_t pos;
} StringArray;

StringArray stra_init(size_t size);
void stra_append(StringArray *array, char *str);
void stra_replace(StringArray *array, size_t n, char *str);
void stra_sort(StringArray *array);
void stra_print(StringArray *array);
void stra_empty(StringArray *array);
void stra_destroy(StringArray *array);

#endif /* STRINGARRAY_H */

#ifdef STRA_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

StringArray stra_init(size_t size)
{
    StringArray array = {
        .data = malloc(size*sizeof(char *)),
        .size = size,
        .pos = 0
    };

    array.data[0] = NULL;
    return array;
}

void stra_append(StringArray *array, char *str)
{
    if (array->pos == array->size-1)
    {
        array->size <<= 1;
        array->data = realloc(array->data, array->size*sizeof(char *));
    }

    array->data[array->pos] = str;
    array->data[array->pos+1] = NULL;
    array->pos++;
}

void stra_replace(StringArray *array, size_t n, char *str)
{
    if (array->pos <= n) return;
    array->data[n] = str;
}

void stra_sort(StringArray *array)
{
    char *tmp = NULL;
    for (size_t i = 0; i < array->pos; i++)
    {
        for (size_t j = 0; j < array->pos; j++)
        {
            if (strcmp(array->data[i], array->data[j]) < 0)
            {
                tmp = array->data[i];
                array->data[i] = array->data[j];
                array->data[j] = tmp;
            }
        }
    }
}

void stra_print(StringArray *array)
{
    for (size_t i = 0; i < array->pos; i++)
        puts(array->data[i]);
}

void stra_empty(StringArray *array)
{
    array->data[0] = NULL;
    array->pos = 0;
}

void stra_destroy(StringArray *array)
{
    array->size = 0;
    array->pos = 0;
    free(array->data);
}

#endif /* STRA_IMPLEMENTATION */
