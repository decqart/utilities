#ifndef STR_H
#define STR_H

#include <stddef.h>
#include <stdbool.h>
#include <Types.h>

typedef struct {
    char *str;
    size_t len;
} Str;

#define NULLSTR (Str) { NULL, 0 }

Str str_init(char *str);
Str str_num_init(char *str, size_t len);
i32 str_index(Str str, char *idx);
void str_reverse(Str *str);
bool str_eq(Str left, Str right);
bool str_startswith(Str str, const char *prefix);
bool str_endswith(Str str, const char *suffix);
void str_destroy(Str *str);

#endif /* STR_H */

#ifdef STR_IMPLEMENTATION

Str str_init(char *str)
{
    return (Str) { str, strlen(str) };
}

Str str_num_init(char *str, size_t len)
{
    return (Str) { str, len };
}

i32 str_index(Str str, char *idx)
{
    if (str.str == NULL) return -1;
    size_t idxlen = strlen(idx);
    for (int i = 0; i < str.len; i++)
    {
        for (int j = 0; j < idxlen; j++)
        {
            if (str.str[i] == idx[j])
                return i;
        }
    }
    return -1;
}

void str_reverse(Str *str)
{
    if (str->str == NULL) return;
    char *tmp = strndup(str->str, str->len);
    if (tmp == NULL) return;
    for (int i = 0; i < str->len; i++)
    {
        str->str[i] = tmp[str->len-1-i];
    }
    free(tmp);
}

bool str_eq(Str left, Str right)
{
    if (left.str == NULL || right.str == NULL)
        return false;
    return !strcmp(left.str, right.str);
}

bool str_startswith(Str str, const char *prefix)
{
    int idx = 0;
    while (prefix[idx] && str.str[idx])
    {
        if (str.str[idx] != prefix[idx])
            return false;
        idx++;
    }
    return true;
}

bool str_endswith(Str str, const char *suffix)
{
    int idx = 0;
    size_t suflen = strlen(suffix);
    while (suffix[idx] && str.str[str.len-suflen+idx])
    {
        if (str.str[str.len-suflen+idx] != suffix[idx])
            return false;
        idx++;
    }
    return true;
}

#endif /* STR_IMPLEMENTATION */
