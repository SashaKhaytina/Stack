#include <stdio.h>
#include <malloc.h>

#include "utils.h"


long long hash_func(void* point, size_t size)
{
    long long hash = 0;
    point = (char*) point;

    for (size_t i = 0; i < size; i++)
    {
        hash += (long long) *((char*)point + i);
    }

    return hash;
}
