#include <stdio.h>
#include <malloc.h>

#include "stack_commands.h"


Error_t default_stack_ctor(Stack* stack, size_t capacity)
{
    // Здесь проверка на NULL // TOD: 
    if (stack == NULL) return STACK_NULL;

    ON_DEBUG(stack->name_current_func = __PRETTY_FUNCTION__;)


    stack->size     = 0;
    stack->capacity = capacity;
    //stack->arr      = (StackElem_t*) (calloc(capacity ON_DEBUG(+ 2), sizeof(StackElem_t)));
    //stack->arr      = (StackElem_t*) (((char*) poison_realloc(stack->arr, 0, (capacity ON_DEBUG(+ 2)) * sizeof(StackElem_t))) + sizeof(StackElem_t)); // Ёпсель-мопсель...

    size_t real_new_size = capacity * sizeof(StackElem_t) ON_DEBUG(+ 2 * sizeof(Canary_t));

    // stack->arr       = (StackElem_t*) (((char*) new_poison_realloc(stack->arr, 0, capacity * sizeof(StackElem_t) ON_DEBUG(+ 2 * sizeof(Canary_t)), sizeof(StackElem_t))) + sizeof(Canary_t)); // Ёпсель-мопсель...  стоит передать sizeof(StackElem_t) и прописывать это все в функции...? 

    stack->arr      = (StackElem_t*) (((char*) new_poison_realloc(stack->arr, 0, real_new_size)) + sizeof(Canary_t)); // Ёпсель-мопсель...  стоит передать sizeof(StackElem_t) и прописывать это все в функции...? 


    ON_DEBUG(*(Canary_t*)((char*) stack->arr - sizeof(Canary_t))               = CANARY;) // TOD: use canary only in debug mode
    ON_DEBUG(*(Canary_t*)((char*) stack->arr + sizeof(StackElem_t) * capacity) = CANARY;) // TODO: canary is not StackElem_t, it's Canary_t

    ON_DEBUG(stack->hash_arr = hash_func(stack->arr, sizeof(StackElem_t) * (stack->capacity));)
    ON_DEBUG(stack->hash_stack = hash_func((void*) stack + sizeof(Hash_t), sizeof(Stack) - sizeof(Hash_t));) // TODO: sizeof(Canary_t), check this in all places in code
    //ON_DEBUG(stack->hash_arr = hash_func(stack->arr, sizeof(StackElem_t) * (stack->size));)

    //print_stack_info(stack); // тут не надо

    // Проверка на корректность
    //return stack_assert(stack); // TOD: wtf
    //return OK; // у него всегда все ок! :)  ..???
    return 0;
}

Error_t stack_dtor(Stack* stack)
{
    ON_DEBUG(stack->name_current_func = __PRETTY_FUNCTION__;)

    
    free((char*) stack->arr - sizeof(StackElem_t));     // Здесь чистить начиная с чего?
    // stack->size = 0;                                    // надо ли это? (для красоты вроде только)
    // stack->capacity = 1;
    //return OK; // И у него всегда все ок! :)  ..???
    return 0;

}

ON_DEBUG
(
Error_t stack_assert(Stack* stack) // TODO: сделай битовыми операциями
{
    Error_t all_errors = 0;

    if (stack == NULL)
    {
        //printf("AAAAAAAAAA1\n");
        //print_stack_info(stack); // TOD: wtf 
        all_errors += STACK_NULL;
        //return STACK_NULL;
    }

    if (stack->arr == NULL)
    {
        //printf("AAAAAAAAAA2\n");

        //print_stack_info(stack);
        all_errors += STACK_ARR_NULL;
        //return STACK_ARR_NULL;
    }

    // if ((stack->size == 0) && (stack->name_current_func == "ProgramStatus stack_pop(Stack*, int)"))
    // {
    //     print_stack_info(stack);
    //     return INCORRECT_POP;
    // }

    if (stack->size > stack->capacity) // размер больше максимального
    {
        //printf("AAAAAAAAAA3\n");
        //print_stack_info(stack);
        all_errors += SIZE_MORE_COMPASITY;
        //return SIZE_MORE_COMPASITY;
    }

    if (stack->capacity > 200000) // Надо ли делать проверку на большое size_t (подозрительно, не переполнение ли?)
    {
        //printf("AAAAAAAAAA4\n");
        all_errors += TOO_LARGE_COMPASITY;
        //print_stack_info(stack);
        //return TOO_LARGE_COMPASITY;
    }

    if (stack->left_canary != CANARY)
    {
        //printf("AAAAAAAAAA5\n");
        all_errors += CHANGE_LEFT_CANARY;
        //print_stack_info(stack);
        //return CHANGE_LEFT_CANARY;
    }

    if (stack->right_canary != CANARY)
    {   
        //printf("AAAAAAAAAA6\n");
        all_errors += CHANGE_RIGHT_CANARY;
        //print_stack_info(stack);
        //return CHANGE_RIGHT_CANARY;
    }

    if (stack->hash_stack != hash_func(((void*) stack + sizeof(Hash_t)), sizeof(Stack) - sizeof(Hash_t))) // тут адекватная запись... она работает что ли?
    {
        //printf("AAAAAAAAAA7\n");
        all_errors += HASH_STACK_ERROR;
        //print_stack_info(stack);
        //return HASH_STACK_ERROR;
    }

    // Оно может быть не создано!
    if ((*(Canary_t*)((char*) stack->arr - sizeof(Canary_t))) != CANARY)
    {
        //printf("AAAAAAAAAA8\n");
        all_errors += CHANGE_LEFT_ARR_CANARY;
        //print_stack_info(stack);
        //return CHANGE_LEFT_ARR_CANARY;
    }

    if (*(Canary_t*)((char*) stack->arr + sizeof(StackElem_t) * stack->capacity) != CANARY)
    {
        //printf("AAAAAAAAAA9\n");
        all_errors += CHANGE_RIGHT_ARR_CANARY;
        //print_stack_info(stack);
        //return CHANGE_RIGHT_ARR_CANARY;
    }

    //if (stack->hash_arr != hash_func(stack->arr, sizeof(StackElem_t) * (stack->size))) // тут отлавливает только данные, без "POISON"
    if (stack->hash_arr != hash_func(stack->arr, sizeof(StackElem_t) * (stack->capacity)))
    {
        all_errors += HASH_ARR_ERROR;
        //print_stack_info(stack);
        //return HASH_ARR_ERROR;
    }

    
    if (all_errors != 0)
        print_stack_info(stack);

    return all_errors;
}
)


void print_stack_info(Stack* stack) // TOO: where is assert? (теперь не нужен)
{
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
    printf("arr[%d] = %d - тут должен быть POISON\n", stack->size, stack->arr[stack->size]);
    //printf("arr[%d] = %d - тут должен быть POISON\n", stack->size + 2, stack->arr[stack->size + 1]);

    ON_DEBUG(printf("right arr canary - %x\n", *(Canary_t*)((char*) stack->arr + sizeof(StackElem_t) * stack->capacity));)

    printf("\n\n");


}


Error_t stack_push(Stack* stack, StackElem_t elem ON_DEBUG(, int code_num_string))
{
    // ON_DEBUG
    // (
    // ProgramStatus stat = stack_assert(stack);
    // if (stat) return stat;
    // )

    // ON_DEBUG(stack->code_num_string = code_num_string;)
    // ON_DEBUG(stack->name_current_func = __PRETTY_FUNCTION__;) 
    ON_DEBUG(CHECK_STACK_INFO)

    if (stack->size == stack->capacity) // TOD: just stack->size == stack->capacity // TOD: do that later, before pushing element // WHY - 1? (Без этого улетает с длинной массива) (не актуально вроде. Надо просто нормально делать вывод) (Актуально только без дебага...............) (Проблема решилась...)
    {
        //stack->arr = (StackElem_t*) poison_realloc((char*) stack->arr - sizeof(StackElem_t), stack->capacity, stack->capacity * 2 * sizeof(StackElem_t)); // Функцию переписать!

        //stack->arr       = (StackElem_t*) (((char*) poison_realloc(&(stack->arr[-1]), (stack->capacity ON_DEBUG(+ 2)) * sizeof(StackElem_t), (2 * stack->capacity ON_DEBUG(+ 2)) * sizeof(StackElem_t))) + sizeof(StackElem_t)); // Ёпсель-мопсель...  стоит передать sizeof(StackElem_t) и прописывать это все в функции...? 
        stack->arr       = (StackElem_t*) (((char*) new_poison_realloc((((char*) stack->arr - sizeof(Canary_t))), stack->capacity * sizeof(StackElem_t) ON_DEBUG(+ 2 * sizeof(Canary_t)), 2 * stack->capacity * sizeof(StackElem_t) ON_DEBUG(+ 2 * sizeof(Canary_t)))) + sizeof(Canary_t)); // Ёпсель-мопсель...  стоит передать sizeof(StackElem_t) и прописывать это все в функции...? 

        stack->capacity *= 2;
        //ON_DEBUG(stack->arr[stack->capacity] = CANARY_ARR;)
        ON_DEBUG(*(Canary_t*)((char*) stack->arr + sizeof(StackElem_t) * stack->capacity)  = CANARY;)
    }
 
    stack->arr[stack->size] = elem;
    stack->size += 1; 
    
    //ON_DEBUG(stack->hash_arr = hash_func(stack->arr, sizeof(StackElem_t) * (stack->size));)
    ON_DEBUG(stack->hash_arr = hash_func(stack->arr, sizeof(StackElem_t) * (stack->capacity));)

    ON_DEBUG(stack->hash_stack = hash_func((void*) stack + sizeof(Hash_t), sizeof(Stack) - sizeof(Hash_t));)

    //print_stack_info(stack); // здесь не надо

    ON_DEBUG(return stack_assert(stack);)
    return 0;
}





Error_t stack_pop(Stack* stack  ON_DEBUG(, int code_num_string))
{
    // // TOD: macro CHECK_STACK_INFO
    // ON_DEBUG
    // (
    // ProgramStatus stat = stack_assert(stack);
    // if (stat) return stat;
    // )

    // ON_DEBUG(stack->code_num_string = code_num_string;)
    // ON_DEBUG(stack->name_current_func = __PRETTY_FUNCTION__;)

    ON_DEBUG(CHECK_STACK_INFO)

    // TOD: check if when you want to pop, your size isn't equal to 0
    if (stack->size == 0) return INCORRECT_POP; // См попытку сделать это в assert

    stack->size -= 1;

    if (stack->size <= (stack->capacity / 4)) // <= так как вдруг нечет
    {
        //stack->arr = (StackElem_t*) realloc(stack->arr, (stack->capacity ON_DEBUG(+ 2)) * sizeof(StackElem_t)); // Функцию переписать!

        // printf("%p - start\n", (StackElem_t*) ((char*)stack->arr - sizeof(StackElem_t)));
        // printf("%p - start\n", stack->arr[-1]);
        

        // stack->arr = (StackElem_t*) (
        //     ((char*) poison_realloc(
        //         &(stack->arr[-1]), 
        //         (stack->capacity ON_DEBUG(+ 2)) * sizeof(StackElem_t), 
        //         (stack->capacity / 2 ON_DEBUG(+ 2)) * sizeof(StackElem_t))) 
        //     + sizeof(StackElem_t)); // Ёпсель-мопсель...

        stack->arr       = (StackElem_t*) (((char*) new_poison_realloc((((char*) stack->arr - sizeof(Canary_t))), stack->capacity * sizeof(StackElem_t) ON_DEBUG(+ 2 * sizeof(Canary_t)), stack->capacity / 2 * sizeof(StackElem_t) ON_DEBUG(+ 2 * sizeof(Canary_t)))) + sizeof(StackElem_t)); // TOD: new size is incorrect // Ёпсель-мопсель...  стоит передать sizeof(StackElem_t) и прописывать это все в функции...? 


        stack->capacity /= 2; 

        // ON_DEBUG(stack->arr[stack->capacity] = CANARY_ARR;)
        ON_DEBUG(*(Canary_t*)((char*) stack->arr + sizeof(StackElem_t) * stack->capacity)  = CANARY;)
    }

    stack->arr[stack->size] = POISON; // тут может быть ошибка!!! ?

    //ON_DEBUG(stack->hash_arr = hash_func(stack->arr, sizeof(StackElem_t) * (stack->size));) // ЭТО ДОЛЖНО БЫТЬ РАНЬШЕ!!! НО ПОЧЕМУ???
    ON_DEBUG(stack->hash_arr = hash_func(stack->arr, sizeof(StackElem_t) * (stack->capacity));)
    ON_DEBUG(stack->hash_stack = hash_func((void*) stack + sizeof(Hash_t), sizeof(Stack) - sizeof(Hash_t));)

    
    //print_stack_info(stack); // здесь не надо

    ON_DEBUG(return stack_assert(stack);)
    return OK;
}
