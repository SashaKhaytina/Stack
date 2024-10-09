#include <stdio.h>
#include <malloc.h>

#include "utils.h"
//#include "stack_commands.h" // ыыыыы

long long hash_func(void* point, int size)
{
    long long hash = 0;
    point = (char*) point;

    for (int i = 0; i < size; i++)
    {
        hash += (long long) *((char*)point + i);
    }
    // int const1 = 13;
    // int const2 = 41;

    // long long hash = 0;
    // point = (char*) point;

    // for (int i = 0; i < size; i++)
    // {
    //     hash = (hash * const1 + (long long) *(((char*)point + i))) % const2;
    // }

    return hash;
}


// С ядом все плохо :(
void* poison_realloc(void* point, size_t old_size, size_t new_size)
{
    if (old_size < new_size)
    {
        void *new_point = realloc(point, new_size);
        // memset((void*) ((char*) new_point + old_size), POISON, new_size - old_size); // Заполняет но ТОЛЬКО 1 БАЙТ...
        // for (int i = 0; i < (new_size - old_size); i+= sizeof(StackElem_t))
        // {
        //     *(StackElem_t*)((char*) new_point + old_size + i) = POISON;
        // }
        return new_point;
    }

    void *new_point = realloc(point, new_size);
    //memset((void*) ((char*) new_point + old_size), POISON, new_size - old_size); // Это тут вообще не надо
    return new_point;
}


void* new_poison_realloc(void* point, size_t old_size, size_t new_size)
{
    // old_size *= elem_type_size;
    // new_size *= elem_type_size;

    if (old_size < new_size)
    {
        void *new_point = realloc(point, new_size);
        // printf("%p - new_point\n", new_point);
        // memset((void*) ((char*) new_point + old_size), POISON, new_size - old_size); // Заполняет но ТОЛЬКО 1 БАЙТ...
        
        // Тут везде долежн быть не double, а StackElem_t

        // for (int i = 0; i < (new_size - old_size); i += sizeof(double))
        // {
        //     printf("AAAAAAAAAAAAA\n");
        //     printf("%p - (double*)(((char*) new_point) + old_size + i)\n", (double*)(((char*) new_point) + old_size + i));
        //     *(double*)(((char*) new_point) + old_size + i) = 1234; // TODO: not StackElem_t, it's elem_type_size
        // }
        return new_point;
    }

    void *new_point = realloc(point, new_size);
    //memset((void*) ((char*) new_point + old_size), POISON, new_size - old_size); // Это тут вообще не надо
    return new_point;
}