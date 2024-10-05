#include <stdio.h>
#include <malloc.h>

#include "utils.h"

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
void* poison_realloc(void* point, size_t old_size, size_t new_size) // TODO: it doesn't work correctly because old_size can be bigger than new_size, check this
{
    printf("%p - point\n", point);
    // void* new_point = memcpy(realloc(point, new_size + 1), point, old_size);     // Копирует
    if (old_size < new_size)
    {
        printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAaa\n");
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
