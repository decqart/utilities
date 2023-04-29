#ifndef STRARRAY_H
#define STRARRAY_H

#include <stddef.h>

typedef struct {
    char **value;
    size_t size;
    size_t pos;
} StrArray;

StrArray stra_init(size_t size);
void stra_append(StrArray *arr, char *str);
void stra_replace(StrArray *array, size_t n, char *str);
void stra_sort(StrArray *array);
void stra_print(StrArray array);
void stra_empty(StrArray *array);
void stra_destroy(StrArray *array);

#endif /* STRARRAY_H */

#ifdef STRA_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

StrArray stra_init(size_t size)
{
    StrArray array;
    array.value = malloc(size*sizeof(char *));
    for (int i = 0; i < size; ++i)
    {
        array.value[i] = NULL;
    }
    array.size = size;
    array.pos = 0;
    return array;
}

void stra_append(StrArray *arr, char *str)
{
    if (arr->value == NULL) return;
    if (arr->pos == arr->size)
    {
        arr->size <<= 1;
        arr->value = realloc(arr->value, arr->size*sizeof(char *));
        for (int i = arr->pos+1; i < arr->size; ++i)
        {
            arr->value[i] = NULL;
        }
    }

    arr->value[arr->pos] = str;
    arr->pos++;
}

void stra_replace(StrArray *array, size_t n, char *str)
{
    if (array->pos < n) return;
    array->value[n] = str;
}

void stra_sort(StrArray *array)
{
    if (array->value == NULL) return;
    char *tmp = NULL;
    for (int i = 0; array->value[i] != NULL; i++)
    {
        for (int j = 0; array->value[j] != NULL; j++)
        {
            if (strcmp(array->value[i], array->value[j]) < 0)
            {
                tmp = array->value[i];
                array->value[i] = array->value[j];
                array->value[j] = tmp;
            }
        }
    }
}

void stra_print(StrArray array)
{
    if (array.value == NULL) return;
    for (int i = 0; array.value[i] != NULL; i++)
    {
        puts(array.value[i]);
    }
}

void stra_empty(StrArray *array)
{
    if (array->value == NULL) return;
    for (int i = 0; i < array->pos; i++)
    {
        array->value[i] = NULL;
    }
    array->pos = 0;
}

void stra_destroy(StrArray *array)
{
    if (array->value == NULL) return;
    array->size = 0;
    array->pos = 0;
    free(array->value);
}

#endif /* STRA_IMPLEMENTATION */
