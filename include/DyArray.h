#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <stdlib.h>

#define DyArray(T) struct {                     \
        T *data;                                \
        size_t size;                            \
        size_t alloced;                         \
    }

#define da_init(T, allocate) {                       \
        .data = malloc(allocate*sizeof(T)),          \
        .size = 0,                                   \
        .alloced = allocate                          \
    }

#define da_passed_init(T, array, allocate) do {                \
        array.data = malloc(allocate*sizeof(T));               \
        array.size = 0;                                        \
        array.alloced = allocate;                              \
    } while (0)

#define da_append(T, array, val) do {                                   \
        if (array.size == array.alloced)                                \
        {                                                               \
            array.alloced <<= 1;                                        \
            array.data = realloc(array.data, array.alloced*sizeof(T));  \
        }                                                               \
        array.data[array.size] = val;                                   \
        array.size++;                                                   \
    } while (0)

#define da_skim(T, array) array.data = realloc(array.data, array.size*sizeof(T))

#define da_destroy(array) do {                  \
        free(array.data);                       \
        array.data = NULL;                      \
        array.size = 0;                         \
        array.alloced = 0;                      \
    } while (0)

#endif /* DYNAMIC_ARRAY_H */
