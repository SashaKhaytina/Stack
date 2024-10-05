#ifndef UTILS
#define UTILS

#include <stdio.h>


typedef long long Hash_t;

Hash_t        hash_func            (void* point, int size); // const void* data - ded wrote
void*         poison_realloc       (void* point, size_t old_size, size_t new_size);


#endif