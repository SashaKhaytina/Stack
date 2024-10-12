#include <stdio.h>
#include <malloc.h>

#include "stack_commands.h"

Error_t default_stack_ctor(Stack* stack, size_t capacity)
{
    if (stack == NULL) return STACK_NULL;

    ON_DEBUG(stack->name_current_func = __PRETTY_FUNCTION__;)

    stack->size     = 0;
    stack->capacity = capacity;

    stack->arr = (StackElem_t*) (ON_DEBUG((char*) )calloc(capacity, sizeof(StackElem_t) ON_DEBUG(+ 2 * sizeof(Canary_t)/capacity)) ON_DEBUG(+ sizeof(Canary_t)));

    stack_memset(stack->arr, 0 ON_DEBUG(+ sizeof(Canary_t)), capacity * sizeof(StackElem_t) ON_DEBUG(+ 2 * sizeof(Canary_t)));

    ON_DEBUG(*(Canary_t*)((char*) stack->arr - sizeof(Canary_t)) = CANARY;)
    ON_DEBUG(*(Canary_t*)((char*) stack->arr + sizeof(StackElem_t) * stack->capacity)  = CANARY;)

    ON_DEBUG(stack->hash_arr = hash_func(stack->arr, sizeof(StackElem_t) * (stack->capacity));)
    ON_DEBUG(stack->hash_stack = hash_func((void*) ((char*) stack + sizeof(Hash_t)), sizeof(Stack) - sizeof(Hash_t));)

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
        all_errors |= STACK_NULL;
        return all_errors;
    }

    if (stack->arr == NULL)
    {
        all_errors |= STACK_ARR_NULL;
        return all_errors;

    }

    if (stack->size > stack->capacity)
    {
        all_errors |= SIZE_MORE_CAPACITY;
    }

    if (stack->capacity > 200000)
    {
        all_errors |= TOO_LARGE_CAPACITY;
    }

    if (stack->left_canary != CANARY)
    {
        all_errors |= CHANGE_LEFT_CANARY;
    }

    if (stack->right_canary != CANARY)
    {   
        all_errors |= CHANGE_RIGHT_CANARY;
    }

    if (stack->hash_stack != hash_func(((void*) ((char*) stack + sizeof(Hash_t))), sizeof(Stack) - sizeof(Hash_t)))
    {
        all_errors |= HASH_STACK_ERROR;
    }

    if ((*(Canary_t*)((char*) stack->arr - sizeof(Canary_t))) != CANARY)
    {
        all_errors |= CHANGE_LEFT_ARR_CANARY;
    }

    if (*(Canary_t*)((char*) stack->arr + sizeof(StackElem_t) * stack->capacity) != CANARY)
    {
        all_errors |= CHANGE_RIGHT_ARR_CANARY;
    }

    if (stack->hash_arr != hash_func(stack->arr, sizeof(StackElem_t) * (stack->capacity)))
    {
        all_errors |= HASH_ARR_ERROR;
    }
    
    if (all_errors != 0) print_stack_info(stack, all_errors);

    return all_errors;
}
)


void print_stack_info(Stack* stack, Error_t status)
{
    ON_DEBUG(print_all_errors(status);)

    ON_DEBUG(printf("from main()::%d -- func - %s\n", stack->code_num_string, stack->name_current_func);)
    ON_DEBUG(printf("hash_stack - %lld\n", stack->hash_stack);)
    ON_DEBUG(printf("hash_arr - %lld\n", stack->hash_arr);)

    ON_DEBUG(printf("left canary - %llx\n", stack->left_canary);)
    ON_DEBUG(printf("right canary - %llx\n", stack->right_canary);)

    printf("capacity - %lu\n", stack->capacity);
    printf("size - %lu\n", stack->size);

    ON_DEBUG(printf("left arr canary - %llx\n", (*(Canary_t*)((char*) stack->arr - sizeof(Canary_t))));)

    for (int i = (int) stack->size - 1; i >= 0; i--)
    {
        printf("arr[%d] = %f\n", i, stack->arr[i]);
    }
    //printf("arr[%d] = %g - тут должен быть POISON\n", stack->size, stack->arr[stack->size]);

    ON_DEBUG(printf("right arr canary - %llx\n", *(Canary_t*)((char*) stack->arr + sizeof(StackElem_t) * stack->capacity));)

    printf("\n\n");
}


Error_t stack_push(Stack* stack, StackElem_t elem ON_DEBUG(, int code_num_string))
{
    CHECK_STACK_INFO

    ON_DEBUG(stack->code_num_string = code_num_string;)
    ON_DEBUG(stack->name_current_func = __PRETTY_FUNCTION__;)
    
    if (stack->size == stack->capacity)
    {
        stack_realloc(stack, 2 * stack->capacity);
    }
 
    stack->arr[stack->size] = elem;
    stack->size += 1; 
    
    ON_DEBUG(stack->hash_arr = hash_func(stack->arr, sizeof(StackElem_t) * (stack->capacity));)
    ON_DEBUG(stack->hash_stack = hash_func((void*) ((char*) stack + sizeof(Hash_t)), sizeof(Stack) - sizeof(Hash_t));)

    ON_DEBUG(status = stack_assert(stack);)
    ON_DEBUG(print_stack_info(stack, status);) // TODO: delete

    ON_DEBUG(return status;)
    return 0;
}


Error_t stack_pop(Stack* stack  ON_DEBUG(, int code_num_string))
{
    CHECK_STACK_INFO

    if (stack->size == 0) return INCORRECT_POP;

    ON_DEBUG(stack->code_num_string = code_num_string;)
    ON_DEBUG(stack->name_current_func = __PRETTY_FUNCTION__;)


    stack->size -= 1;

    if (stack->size <= (stack->capacity / 4))
    {
        stack_realloc(stack, stack->capacity / 2);
    }

    stack->arr[stack->size] = POISON; // тут может быть ошибка?

    ON_DEBUG(stack->hash_arr = hash_func(stack->arr, sizeof(StackElem_t) * (stack->capacity));)
    ON_DEBUG(stack->hash_stack = hash_func((void*) ((char*) stack + sizeof(Hash_t)), sizeof(Stack) - sizeof(Hash_t));)

    ON_DEBUG(status = stack_assert(stack);)
    ON_DEBUG(print_stack_info(stack, status);)


    ON_DEBUG(return status;)
    return 0;
}


void stack_realloc(Stack* stack, size_t new_size) 
{
    new_size        = new_size        * sizeof(StackElem_t) ON_DEBUG(+ 2 * sizeof(Canary_t));
    size_t old_size = stack->capacity * sizeof(StackElem_t) ON_DEBUG(+ 2 * sizeof(Canary_t));

    StackElem_t* point = (StackElem_t*) (ON_DEBUG((char*) )stack->arr ON_DEBUG(- sizeof(Canary_t)));

    
    if (old_size < new_size)
    {
        point = (StackElem_t*) realloc(point, new_size);

        stack_memset(point, old_size, new_size);
        
        stack->arr = (StackElem_t*) (ON_DEBUG((char*) )point ON_DEBUG(+ sizeof(Canary_t)));


        stack->capacity *= 2;

        ON_DEBUG(*(Canary_t*)((char*) stack->arr + sizeof(StackElem_t) * stack->capacity)  = CANARY;)
    }
    else
    {
        stack->arr = (StackElem_t*) ((char*) realloc(point, new_size) ON_DEBUG(+ sizeof(Canary_t)));

        stack->capacity /= 2;
        ON_DEBUG(*(Canary_t*)((char*) stack->arr + sizeof(StackElem_t) * stack->capacity)  = CANARY;)
    }
}


void stack_memset(StackElem_t* point, size_t old_size, size_t new_size)
{
    for (size_t i = old_size ON_DEBUG(- sizeof(Canary_t)); i < new_size ON_DEBUG(- sizeof(Canary_t)); i += sizeof(StackElem_t)) 
    {
        *(StackElem_t*)((char*) point + i) = POISON;
    }

    // for (size_t i = old_size ON_DEBUG(- sizeof(Canary_t)); i < new_size ON_DEBUG(- sizeof(Canary_t)); i += sizeof(StackElem_t)) 
    // {
    //     *(StackElem_t*)((char*) point + i) = POISON;
    // } 

    // for (int i = 0; i < (new_size - old_size ON_DEBUG(- sizeof(Canary_t))); i += sizeof(StackElem_t))
    // {
    //     *(StackElem_t*)(((char*) point) + old_size + i) = POISON;
    // }
}
