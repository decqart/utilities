#ifndef STRARRAY_H
#define STRARRAY_H

#include <stdlib.h>
#include <stddef.h>

typedef struct {
    char **value;
    size_t size;
    size_t pos;
} StrArray;

StrArray stra_init(void)
{
    StrArray array;
    array.value = malloc(50*sizeof(char *));
    for (int i = 0; i < 50; ++i)
    {
        array.value[i] = NULL;
    }
    array.size = 50;
    array.pos = 0;
    return array;
}

void stra_append(StrArray *arr, char *str)
{
    if (arr->pos == arr->size)
    {
        arr->size <<= 1;
        arr->value = realloc(arr->value, arr->size*sizeof(char *));
    }

    arr->value[arr->pos] = str;
    arr->pos++;
}

void stra_sort(StrArray *array)
{
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

void stra_destroy(StrArray *array)
{
    if (array->value == NULL) return;
    array->size = 0;
    array->pos = 0;
    free(array->value);
}

#endif /* STRARRAY_H*/
