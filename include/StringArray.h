#ifndef STRING_ARRAY_H
#define STRING_ARRAY_H

#include <stdlib.h>
#include <string.h>

typedef struct {
    char **data;
    size_t size;
    size_t alloced;
} StringArray;

static inline StringArray stra_init(size_t allocate)
{
    return (StringArray) {
        .data = malloc(allocate*sizeof(char *)),
        .size = 0,
        .alloced = allocate
    };
}

static inline void stra_append(StringArray *array, char *val)
{
    if (array->size == array->alloced)
    {
        array->alloced <<= 1;
        array->data = realloc(array->data, array->alloced*sizeof(char *));
    }
    array->data[array->size] = val;
    array->size++;
}

static inline void stra_sort(StringArray *array)
{
    for (size_t i = 0; i < array->size; ++i)
    {
        for (size_t j = 0; j < array->size; ++j)
        {
            if (strcmp(array->data[i], array->data[j]) < 0)
            {
                char *tmp = array->data[i];
                array->data[i] = array->data[j];
                array->data[j] = tmp;
            }
        }
    }
}

static inline void stra_skim(StringArray *array)
{
    array->data = realloc(array->data, array->size*sizeof(char *));
    array->alloced = array->size;
}

static inline void stra_destroy(StringArray *array)
{
    free(array->data);
    array->data = NULL;
    array->size = 0;
    array->alloced = 0;
}

#endif /* STRING_ARRAY_H */
