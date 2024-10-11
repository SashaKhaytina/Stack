#include <stdio.h>
#include <malloc.h>

#include "stack_commands.h"


Error_t default_stack_ctor(Stack* stack, size_t capacity)
{
    if (stack == NULL) return STACK_NULL;

    ON_DEBUG(stack->name_current_func = __PRETTY_FUNCTION__;)

    stack->size     = 0;
    stack->capacity = capacity;

    size_t real_new_size = capacity * sizeof(StackElem_t) ON_DEBUG(+ 2 * sizeof(Canary_t));

    // stack->arr = (StackElem_t*) (((char*) stack_realloc(
    //                                                         stack->arr,
    //                                                         0, 
    //                                                         capacity * sizeof(StackElem_t) ON_DEBUG(+ 2 * sizeof(Canary_t)))) 
    //                             + sizeof(Canary_t));


    // ON_DEBUG(*(Canary_t*)((char*) stack->arr - sizeof(Canary_t))               = CANARY;)
    // ON_DEBUG(*(Canary_t*)((char*) stack->arr + sizeof(StackElem_t) * capacity) = CANARY;)

    stack_realloc(stack, capacity);

    ON_DEBUG(stack->hash_arr = hash_func(stack->arr, sizeof(StackElem_t) * (stack->capacity));)
    ON_DEBUG(stack->hash_stack = hash_func((void*) stack + sizeof(Hash_t), sizeof(Stack) - sizeof(Hash_t));)

    return 0;
}


Error_t stack_dtor(Stack* stack)
{
    ON_DEBUG(stack->name_current_func = __PRETTY_FUNCTION__;)

    free(ON_DEBUG((char*) )stack->arr ON_DEBUG(- sizeof(StackElem_t)));
    return 0;
}


ON_DEBUG
(
Error_t stack_assert(Stack* stack)
{
    Error_t all_errors = 0;

    if (stack == NULL)
    { 
        all_errors |= STACK_NULL; // TODO: при такой ошибке не нужно проверять остальное
        return all_errors;
    }

    if (stack->arr == NULL)
    {
        all_errors |= STACK_ARR_NULL;
        return all_errors;
        // TODO: сделать через логическое или
    }

    // if ((stack->size == 0) && (stack->name_current_func == "ProgramStatus stack_pop(Stack*, int)"))
    // {
    //     print_stack_info(stack);
    //     return INCORRECT_POP;
    // }

    if (stack->size > stack->capacity)
    {
        all_errors |= SIZE_MORE_CAPASITY; // TODO: wtf is compasity
    }

    if (stack->capacity > 200000)
    {
        all_errors |= TOO_LARGE_CAPASITY;
    }

    if (stack->left_canary != CANARY)
    {
        all_errors |= CHANGE_LEFT_CANARY;
    }

    if (stack->right_canary != CANARY)
    {   
        all_errors |= CHANGE_RIGHT_CANARY;
    }

    if (stack->hash_stack != hash_func(((void*) stack + sizeof(Hash_t)), sizeof(Stack) - sizeof(Hash_t))) // тут адекватная запись... она работает что ли?
    {
        all_errors |= HASH_STACK_ERROR;
    }

    // Оно может быть не создано!
    if ((*(Canary_t*)((char*) stack->arr - sizeof(Canary_t))) != CANARY)
    {
        all_errors |= CHANGE_LEFT_ARR_CANARY;
    }

    if (*(Canary_t*)((char*) stack->arr + sizeof(StackElem_t) * stack->capacity) != CANARY)
    {
        all_errors |= CHANGE_RIGHT_ARR_CANARY;
    }

    //if (stack->hash_arr != hash_func(stack->arr, sizeof(StackElem_t) * (stack->size))) // тут отлавливает только данные, без "POISON"
    if (stack->hash_arr != hash_func(stack->arr, sizeof(StackElem_t) * (stack->capacity)))
    {
        all_errors |= HASH_ARR_ERROR;
    }

    
    if (all_errors != 0)
        print_stack_info(stack, all_errors);

    return all_errors;
}
)


void print_stack_info(Stack* stack, Error_t status) // TOO: where is assert? (теперь не нужен) // TODO: вообще тут нужно выдавать если была ошибка, или если ее не было
{
    ON_DEBUG(print_all_errors(status);)

    ON_DEBUG(printf("from main()::%d -- func - %s\n", stack->code_num_string, stack->name_current_func);)
    ON_DEBUG(printf("hash_stack - %lld\n", stack->hash_stack);)
    ON_DEBUG(printf("hash_arr - %lld\n", stack->hash_arr);)

    ON_DEBUG(printf("left canary - %x\n", stack->left_canary);)
    ON_DEBUG(printf("right canary - %x\n", stack->right_canary);)

    printf("capacity - %lu\n", stack->capacity);
    printf("size - %lu\n", stack->size);

    ON_DEBUG(printf("left arr canary - %x\n", (*(Canary_t*)((char*) stack->arr - sizeof(Canary_t))));)

    for (int i = (int) stack->size - 1; i >= 0; i--)
    {
        printf("arr[%d] = %f\n", i, stack->arr[i]);
    }
    //printf("arr[%d] = %d - тут должен быть POISON\n", stack->size + 2, stack->arr[stack->size + 1]);

    ON_DEBUG(printf("right arr canary - %x\n", *(Canary_t*)((char*) stack->arr + sizeof(StackElem_t) * stack->capacity));)

    printf("\n\n");
}


Error_t stack_push(Stack* stack, StackElem_t elem ON_DEBUG(, int code_num_string))
{
    ON_DEBUG(CHECK_STACK_INFO)

    if (stack->size == stack->capacity)
    {
        // stack->arr = (StackElem_t*) (((char*) stack_realloc(
        //                                             (char*) stack->arr - sizeof(Canary_t), 
        //                                             stack->capacity * sizeof(StackElem_t) ON_DEBUG(+ 2 * sizeof(Canary_t)), 
        //                                             2 * stack->capacity * sizeof(StackElem_t) ON_DEBUG(+ 2 * sizeof(Canary_t)))) 
        //                             + sizeof(Canary_t)); 

        // stack->capacity *= 2;
        // ON_DEBUG(*(Canary_t*)((char*) stack->arr + sizeof(StackElem_t) * stack->capacity)  = CANARY;)
        stack_realloc(stack, 2 * stack->capacity);
    }
 
    stack->arr[stack->size] = elem;
    stack->size += 1; 
    
    ON_DEBUG(stack->hash_arr = hash_func(stack->arr, sizeof(StackElem_t) * (stack->capacity));)
    ON_DEBUG(stack->hash_stack = hash_func((void*) stack + sizeof(Hash_t), sizeof(Stack) - sizeof(Hash_t));)

    ON_DEBUG(status = stack_assert(stack);)
    ON_DEBUG(print_stack_info(stack, status);)

    ON_DEBUG(return status;)
    return 0;
}


Error_t stack_pop(Stack* stack  ON_DEBUG(, int code_num_string))
{

    ON_DEBUG(CHECK_STACK_INFO)

    if (stack->size == 0) return INCORRECT_POP;

    stack->size -= 1;

    if (stack->size <= (stack->capacity / 4)) // <= так как вдруг нечет
    {
        // stack->arr = (StackElem_t*) (((char*) stack_realloc(
        //                                         ((char*) stack->arr - sizeof(Canary_t)), 
        //                                         stack->capacity * sizeof(StackElem_t) ON_DEBUG(+ 2 * sizeof(Canary_t)), 
        //                                         stack->capacity / 2 * sizeof(StackElem_t) ON_DEBUG(+ 2 * sizeof(Canary_t))))
        //                             + sizeof(StackElem_t)); 

        // stack->capacity /= 2; 

        // ON_DEBUG(*(Canary_t*)((char*) stack->arr + sizeof(StackElem_t) * stack->capacity)  = CANARY;)

        stack_realloc(stack, stack->capacity / 2);
    }

    stack->arr[stack->size] = POISON; // тут может быть ошибка!!! ?

    ON_DEBUG(stack->hash_arr = hash_func(stack->arr, sizeof(StackElem_t) * (stack->capacity));)
    ON_DEBUG(stack->hash_stack = hash_func((void*) stack + sizeof(Hash_t), sizeof(Stack) - sizeof(Hash_t));)

    ON_DEBUG(status = stack_assert(stack);)
    ON_DEBUG(print_stack_info(stack, status);)


    ON_DEBUG(return status;)
    return 0;
}


void stack_realloc(Stack* stack, size_t new_size) // TODO: rename to stack_realloc, params of function: void*, size_t old_capacity, size_t new_capacity
{
    //printf("%d - new_size start \n", new_size);
    new_size        = new_size        * sizeof(StackElem_t) ON_DEBUG(+ 2 * sizeof(Canary_t));
    //printf("%d - new_size finish \n", new_size);
    size_t old_size = 0;

    StackElem_t* point = NULL;
    
    if(stack->arr == NULL)
    {
        //printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
        point = stack->arr;
    }
    else 
    {
        //printf("BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB\n");
        point = (StackElem_t*) (ON_DEBUG((char*) )stack->arr ON_DEBUG(- sizeof(Canary_t)));
        old_size = stack->capacity * sizeof(StackElem_t) ON_DEBUG(+ sizeof(Canary_t));
    }

    //printf("%d - old size \n", old_size);

    if (old_size < new_size ON_DEBUG(- sizeof(Canary_t)))
    {
        //point = (StackElem_t*) ((char*) realloc(point, new_size) ON_DEBUG(+ sizeof(Canary_t)));

        point = (StackElem_t*) realloc(point, new_size);

        for (int i = 0; i < (new_size - old_size ON_DEBUG(- sizeof(Canary_t))); i += sizeof(StackElem_t))
        {
            *(StackElem_t*)(((char*) point) + old_size + i) = POISON; // TODO: not StackElem_t, it's elem_type_size
        }
        
        stack->arr = (StackElem_t*) (ON_DEBUG((char*) )point ON_DEBUG(+ sizeof(Canary_t)));

        if (old_size != 0) stack->capacity *= 2;
        else {ON_DEBUG(*(Canary_t*)((char*) stack->arr - sizeof(Canary_t)) = CANARY;)}

        //printf("%d - capacity\n", stack->capacity);


        ON_DEBUG(*(Canary_t*)((char*) stack->arr + sizeof(StackElem_t) * stack->capacity)  = CANARY;)

    }
    else
    {
        stack->arr = (StackElem_t*) ((char*) realloc(point, new_size) ON_DEBUG(+ sizeof(Canary_t))); // TODO: тут ты не кладешь в конец канарейки

        stack->capacity /= 2;
        ON_DEBUG(*(Canary_t*)((char*) stack->arr + sizeof(StackElem_t) * stack->capacity)  = CANARY;)
    }
}
