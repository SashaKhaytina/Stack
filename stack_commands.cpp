#include <stdio.h>
#include <malloc.h>

#include "stack_commands.h"


ProgramStatus default_stack_ctor(Stack* stack, size_t capacity)
{
    // Здесь проверка на NULL
    ON_DEBUG(stack->name_current_func = __PRETTY_FUNCTION__;)


    stack->size     = 0;
    stack->capacity = capacity;
    //stack->arr      = (StackElem_t*) (calloc(capacity ON_DEBUG(+ 2), sizeof(StackElem_t)));
    stack->arr      = (StackElem_t*) (((char*) poison_realloc(stack->arr, 0, (capacity ON_DEBUG(+ 2)) * sizeof(StackElem_t))) + sizeof(StackElem_t)); // Ёпсель-мопсель...

    ON_DEBUG(stack->arr[-1]              = CANARY_ARR;) // TOD: use canary only in debug mode
    ON_DEBUG(stack->arr[stack->capacity] = CANARY_ARR;)

    ON_DEBUG(stack->hash_arr = hash_func(stack->arr, sizeof(StackElem_t) * (stack->capacity));)
    ON_DEBUG(stack->hash_stack = hash_func((void*) stack + sizeof(Hash_t), sizeof(Stack) - sizeof(Hash_t));) // TODO: sizeof(Canary_t), check this in all places in code
    //ON_DEBUG(stack->hash_arr = hash_func(stack->arr, sizeof(StackElem_t) * (stack->size));)

    //print_stack_info(stack); // тут не надо

    // Проверка на корректность
    //return stack_assert(stack); // TOD: wtf
    return OK; // у него всегда все ок! :)  ..???
}

ProgramStatus stack_dtor(Stack* stack)
{
    ON_DEBUG(stack->name_current_func = __PRETTY_FUNCTION__;)

    
    free((char*) stack->arr - sizeof(StackElem_t));     // Здесь чистить начиная с чего?
    // stack->size = 0;                                    // надо ли это? (для красоты вроде только)
    // stack->capacity = 1;
    return OK; // И у него всегда все ок! :)  ..???

}

ON_DEBUG
(
ProgramStatus stack_assert(Stack* stack)
{
    if (stack == NULL)
    {
        print_stack_info(stack);
        return STACK_NULL;
    }

    if (stack->arr == NULL)
    {
        print_stack_info(stack);
        return STACK_ARR_NULL;
    }

    if (stack->size > stack->capacity) // размер больше максимального
    {
        print_stack_info(stack);
        return SIZE_MORE_COMPASITY;
    }

    if (stack->capacity > 200000) // Надо ли делать проверку на большое size_t (подозрительно, не переполнение ли?)
    {
        print_stack_info(stack);
        return TOO_LARGE_COMPASITY;
    }

    if (stack->left_canary != CANARY)
    {
        print_stack_info(stack);
        return CHANGE_LEFT_CANARY;
    }

    if (stack->right_canary != CANARY)
    {
        print_stack_info(stack);
        return CHANGE_RIGHT_CANARY;
    }

    if (stack->hash_stack != hash_func(((void*) stack + sizeof(Hash_t)), sizeof(Stack) - sizeof(Hash_t))) // тут адекватная запись... она работает что ли?
    {
        print_stack_info(stack);
        return HASH_STACK_ERROR;
    }

    // Оно может быть не создано!
    if (stack->arr[-1] != CANARY_ARR)
    {
        print_stack_info(stack);
        return CHANGE_LEFT_ARR_CANARY;
    }

    if (stack->arr[stack->capacity] != CANARY_ARR)
    {
        print_stack_info(stack);
        return CHANGE_RIGHT_ARR_CANARY;
    }

    //if (stack->hash_arr != hash_func(stack->arr, sizeof(StackElem_t) * (stack->size))) // тут отлавливает только данные, без "POISON"
    if (stack->hash_arr != hash_func(stack->arr, sizeof(StackElem_t) * (stack->capacity)))
    {
        print_stack_info(stack);
        return HASH_ARR_ERROR;
    }

    return OK;
}
)


void print_stack_info(Stack* stack)
{
    ON_DEBUG(printf("from main()::%d -- func - %s\n", stack->code_num_string, stack->name_current_func);)
    ON_DEBUG(printf("hash_stack - %lld\n", stack->hash_stack);)
    ON_DEBUG(printf("hash_arr - %lld\n", stack->hash_arr);)

    ON_DEBUG(printf("left canary - %x\n", stack->left_canary);)
    ON_DEBUG(printf("right canary - %x\n", stack->right_canary);)

    printf("capacity - %lu\n", stack->capacity);
    printf("size - %lu\n", stack->size);

    ON_DEBUG(printf("left arr canary - %f\n", stack->arr[-1]);)

    for (int i = (int) stack->size - 1; i >= 0; i--)
    {
        printf("arr[%d] = %f\n", i, stack->arr[i]);
    }
    //printf("arr[%d] = %d - тут должен быть POISON\n", stack->size, stack->arr[stack->size]);
    //printf("arr[%d] = %d - тут должен быть POISON\n", stack->size + 2, stack->arr[stack->size + 1]);

    ON_DEBUG(printf("right arr canary - %f\n", stack->arr[stack->capacity]);)

    printf("\n\n");


}


ProgramStatus stack_push(Stack* stack, StackElem_t elem ON_DEBUG(, int code_num_string))
{
    ON_DEBUG
    (
    ProgramStatus stat = stack_assert(stack);
    if (stat) return stat;
    )

    ON_DEBUG(stack->code_num_string = code_num_string;)
    ON_DEBUG(stack->name_current_func = __PRETTY_FUNCTION__;) 

    if (stack->size == stack->capacity - 1) // TOD: do that later, before pushing element // WHY - 1? (Без этого улетает с длинной массива) (не актуально вроде. Надо просто нормально делать вывод) (Актуально только без дебага...............)
    {
        //stack->arr = (StackElem_t*) poison_realloc((char*) stack->arr - sizeof(StackElem_t), stack->capacity, stack->capacity * 2 * sizeof(StackElem_t)); // Функцию переписать!

        stack->arr       = (StackElem_t*) (((char*) poison_realloc((StackElem_t*) ((char*)stack->arr - sizeof(StackElem_t)), (stack->capacity ON_DEBUG(+ 2)) * sizeof(StackElem_t), (2 * stack->capacity ON_DEBUG(+ 2)) * sizeof(StackElem_t))) + sizeof(StackElem_t)); // Ёпсель-мопсель...

        stack->capacity *= 2;
        // TODO: do recalloc function
        ON_DEBUG(stack->arr[stack->capacity] = CANARY_ARR;)
    }
 
    // TODO: проверь логику для size
    //stack->size += 1; // TOD: когда ты заходишь в функцию, size должен уже быть там, куда ты должна пушить новый элемент
    stack->arr[stack->size] = elem;
    stack->size += 1; // TOD: правильно увеличивать size вот здесь, а не раньше
    
    //ON_DEBUG(stack->hash_arr = hash_func(stack->arr, sizeof(StackElem_t) * (stack->size));)
    ON_DEBUG(stack->hash_arr = hash_func(stack->arr, sizeof(StackElem_t) * (stack->capacity));)

    ON_DEBUG(stack->hash_stack = hash_func((void*) stack + sizeof(Hash_t), sizeof(Stack) - sizeof(Hash_t));)

    print_stack_info(stack); // здесь не надо

    ON_DEBUG(return stack_assert(stack);)
    return OK;
}





ProgramStatus stack_pop(Stack* stack  ON_DEBUG(, int code_num_string))
{
    ON_DEBUG
    (
    ProgramStatus stat = stack_assert(stack);
    if (stat) return stat;
    )

    ON_DEBUG(stack->code_num_string = code_num_string;)
    ON_DEBUG(stack->name_current_func = __PRETTY_FUNCTION__;)

    stack->size -= 1;

    if (stack->size <= (stack->capacity / 4)) // <= так как вдруг нечет
    {
        //stack->arr = (StackElem_t*) realloc(stack->arr, (stack->capacity ON_DEBUG(+ 2)) * sizeof(StackElem_t)); // Функцию переписать!

        printf("%p - start\n", (StackElem_t*) ((char*)stack->arr - sizeof(StackElem_t)));
        printf("%p - start\n", stack->arr[-1]);
        

        stack->arr = (StackElem_t*) (
            ((char*) poison_realloc(
                &(stack->arr[-1]), 
                (stack->capacity ON_DEBUG(+ 2)) * sizeof(StackElem_t), 
                (stack->capacity / 2 ON_DEBUG(+ 2)) * sizeof(StackElem_t))) 
            + sizeof(StackElem_t)); // Ёпсель-мопсель...

        stack->capacity /= 2; 

         ON_DEBUG(stack->arr[stack->capacity] = CANARY_ARR;)
    }

    stack->arr[stack->size] = POISON; // тут может быть ошибка!!! ?

    //ON_DEBUG(stack->hash_arr = hash_func(stack->arr, sizeof(StackElem_t) * (stack->size));) // ЭТО ДОЛЖНО БЫТЬ РАНЬШЕ!!! НО ПОЧЕМУ???
    ON_DEBUG(stack->hash_arr = hash_func(stack->arr, sizeof(StackElem_t) * (stack->capacity));)
    ON_DEBUG(stack->hash_stack = hash_func((void*) stack + sizeof(Hash_t), sizeof(Stack) - sizeof(Hash_t));)

    
    print_stack_info(stack); // здесь не надо

    ON_DEBUG(return stack_assert(stack);)
    return OK;
}
