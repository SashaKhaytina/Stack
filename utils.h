#ifndef UTILS
#define UTILS

#include <stdio.h>


typedef long long Hash_t;

Hash_t hash_func (void* point, size_t size); // const void* data - ded wrote

// Что-то одно надо оставить
// void*  poison_realloc    (void* point, size_t old_size, size_t new_size);
// void*  new_poison_realloc(void* point, size_t old_size, size_t new_size); 
// void   new_poison_realloc(Stack* stack, size_t new_size);


#endif