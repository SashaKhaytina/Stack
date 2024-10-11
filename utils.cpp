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













// void* new_poison_realloc(void* point, size_t old_size, size_t new_size) // TOD: rename to stack_realloc, params of function: void*, size_t old_capacity, size_t new_capacity
// {
//     // old_size *= elem_type_size;
//     // new_size *= elem_type_size;

//     if (old_size < new_size)
//     {
//         void *new_point = realloc(point, new_size);
//         // printf("%p - new_point\n", new_point);
//         // memset((void*) ((char*) new_point + old_size), POISON, new_size - old_size); // Заполняет но ТОЛЬКО 1 БАЙТ...
        
//         // Тут везде долежн быть не double, а StackElem_t

//         for (int i = 0; i < (new_size - old_size); i += sizeof(StackElem_t))
//         {
//             //printf("AAAAAAAAAAAAA\n");
//             //printf("%p - (double*)(((char*) new_point) + old_size + i)\n", (double*)(((char*) new_point) + old_size + i));
//             *(StackElem_t*)(((char*) new_point) + old_size + i) = 1234; 
//         }
//         return new_point;
//     }

//     void *new_point = realloc(point, new_size); // TOD: тут ты не кладешь в конец канарейки
//     return new_point;
// }


// void new_poison_realloc(Stack* stack, size_t new_size) // TOD: rename to stack_realloc, params of function: void*, size_t old_capacity, size_t new_capacity
// {
//     new_size        = new_size        * sizeof(StackElem_t) ON_DEBUG(+ 2 * sizeof(Canary_t));
//     size_t old_size = 0;
//     StackElem_t* point = NULL;
    
//     if(stack->arr == NULL)
//     {
//         point = stack->arr;
//     }
//     else 
//     {
//     point = ON_DEBUG((char*) )stack->arr ON_DEBUG(- sizeof(Canary_t));
//     size_t old_size = stack->capacity * sizeof(StackElem_t) ON_DEBUG(+ sizeof(Canary_t));
//     }

//     if (old_size < new_size)
//     {
//         //point = (StackElem_t*) ((char*) realloc(point, new_size) ON_DEBUG(+ sizeof(Canary_t)));

//         point = (StackElem_t*) realloc(point, new_size);

//         for (int i = 0; i < (new_size - old_size ON_DEBUG(- sizeof(Canary_t))); i += sizeof(StackElem_t))
//         {
//             *(StackElem_t*)(((char*) point) + old_size + i) = POISON; // TOD: not StackElem_t, it's elem_type_size
//         }
        
//         stack->arr = point ON_DEBUG(+ sizeof(Canary_t));

//         stack->capacity *= 2;

//         ON_DEBUG(*(Canary_t*)((char*) stack->arr + sizeof(StackElem_t) * stack->capacity)  = CANARY;) MMMMMMMEEEEOOOOOOOOOOWWWWWWW

//     }
//     else
//     {
//         stack->arr = (StackElem_t*) realloc(point, new_size) ON_DEBUG(+ sizeof(Canary_t)); // TOD: тут ты не кладешь в конец канарейки

//         stack->capacity /= 2;
//         ON_DEBUG(*(Canary_t*)((char*) stack->arr + sizeof(StackElem_t) * stack->capacity)  = CANARY;)
//     }


//     //stack->arr = (StackElem_t*) realloc(stack->arr, new_size); // TOD: тут ты не кладешь в конец канарейки
// }