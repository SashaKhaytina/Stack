#define DEBUG

#ifdef DEBUG
#define ON_DEBUG(code) code

#else
#define ON_DEBUG(code)

#endif


#include <stdio.h>
#include <assert.h>
#include <malloc.h>

#define PRINTF_RED(string, ...)     printf("\x1b[31m" string "\x1b[0m", ##__VA_ARGS__)
#define PRINTF_GREEN(string, ...)   printf("\x1b[32m" string "\x1b[0m", ##__VA_ARGS__)

#define IF_OK(status) if (!status) status = 
#define ___ ,  // ... НЕ ОЧЕНЬ ...

const size_t COP_START = 16; // Это вообще норм?

typedef double StackElem_t;

struct Stack
{
    StackElem_t* arr;
    size_t       size;
    size_t       compacity;

    ON_DEBUG(int num_str_in_main;)
    ON_DEBUG(char* name_current_func;)
};

enum ProgramStatus {
    OK,
    STACK_NULL, 
    STACK_ARR_NULL,
    SIZE_MORE_COMPASITY, 
    TOO_LARGE_COMPASITY
};


// ProgramStatus stack_ctor(Stack* stack, StackElem_t* arr, size_t size, size_t compacity);

ProgramStatus default_stack_ctor(Stack* stack);
ProgramStatus stack_assert      (Stack* stack);
void          print_stack_info  (Stack* stack);
void          print_error       (ProgramStatus status);

ProgramStatus stack_dtor        (Stack* stack);

ProgramStatus stack_push        (Stack* stack, StackElem_t elem ON_DEBUG(___ int num_str_in_main));
ProgramStatus stack_pop         (Stack* stack  ON_DEBUG(___ int num_str_in_main));


// ХЕШ, вывод в файл, Unitest, warnings

int main()
{
    Stack stack = {};

    ON_DEBUG(printf("DEBUG включен!\n\n");)

    ProgramStatus status = OK; // надо ли его оборачивать в ON_DEBUG?

    IF_OK(status) default_stack_ctor(&stack);

    IF_OK(status) stack_push(&stack, 5 ON_DEBUG(___ __LINE__));
    IF_OK(status) stack_push(&stack, 1 ON_DEBUG(___ __LINE__));
    IF_OK(status) stack_push(&stack, 3 ON_DEBUG(___ __LINE__));

    IF_OK(status) stack_pop(&stack ON_DEBUG(___ __LINE__));
    IF_OK(status) stack_pop(&stack ON_DEBUG(___ __LINE__));
    IF_OK(status) stack_pop(&stack ON_DEBUG(___ __LINE__));
    // IF_OK(status) stack_pop(&stack ON_DEBUG(___ __LINE__)); // compacity - 2, size - 18446744073709551615 (надо проверять на адекватный размер и соответствие с compacity)

    IF_OK(status) stack_push(&stack, 5 ON_DEBUG(___ __LINE__));
    IF_OK(status) stack_push(&stack, 1 ON_DEBUG(___ __LINE__));
    IF_OK(status) stack_push(&stack, 3 ON_DEBUG(___ __LINE__));
    IF_OK(status) stack_push(&stack, 10 ON_DEBUG(___ __LINE__));


    stack_dtor(&stack);

    print_error(status); // Тут "крах системы" (цитата), если ранее не закаллочен
}

// Ее можно сделать, засунув "код main()" в функцию (а создавать внутри все равно default)

// ProgramStatus stack_ctor(Stack* stack, StackElem_t* arr, size_t size, size_t compacity) // не работает для дураков, только для умных
// {
//     // если длина данного массива, compacity и size не соотносятся, будет лажа
//     stack->size      = size;
//     stack->compacity = compacity; // надо ли это вообще вводить? Или лучше рассчитывать из size

//     stack->arr       = (StackElem_t*) realloc(stack->arr, compacity * sizeof(StackElem_t));
//     // и кладем циклом.


//     // Проверка на корректность
//     return stack_assert(stack);


// }


ProgramStatus default_stack_ctor(Stack* stack)
{
    ON_DEBUG(stack->name_current_func = "default_stack_ctor";)


    stack->size      = 0;
    stack->compacity = COP_START;
    stack->arr       = (StackElem_t*) calloc(COP_START, sizeof(StackElem_t));
       

    print_stack_info(stack);

    // Проверка на корректность
    return stack_assert(stack);
}

ProgramStatus stack_dtor(Stack* stack)
{
    ON_DEBUG(stack->name_current_func = "stack_dtor";)

    
    free(stack->arr);
    stack->size = 0;      // надо ли это? (для красоты вроде только)
    stack->compacity = 0;

    return stack_assert(stack);

}


// Тут же не надо assert... Зачем вообще где-либо делать assert?
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

    if (stack->size >= stack->compacity) // размер больше максимального
    {
        print_stack_info(stack);
        return SIZE_MORE_COMPASITY;
    }

    if (stack->compacity > 200000) // Надо ли делать проверку на большое size_t (подозрительно, не переполнение ли?)
    {
        print_stack_info(stack);
        return TOO_LARGE_COMPASITY;
    }
    return OK;
}


void print_stack_info(Stack* stack)  // это в define?.. 
{
    ON_DEBUG(printf("from main()::%d -- func - %s\n", stack->num_str_in_main, stack->name_current_func);)


    printf("compacity - %lu\n", stack->compacity);
    printf("size - %lu\n", stack->size);

    for (int i = (int) stack->size - 1; i >= 0; i--)
    {
        printf("arr[%d] = %f\n", i, stack->arr[i]);
    }

    printf("\n\n");

}


ProgramStatus stack_push(Stack* stack, StackElem_t elem ON_DEBUG(___ int num_str_in_main))
{
    ON_DEBUG(stack->num_str_in_main = num_str_in_main;)
    ON_DEBUG(stack->name_current_func = "stack_push";)


    stack->arr[stack->size] = elem;
    stack->size += 1;

    if (stack->size == stack->compacity)
    {
        stack->compacity *= 2;
        stack->arr = (StackElem_t*) realloc(stack->arr, stack->compacity * sizeof(StackElem_t)); // Функцию переписать!
    }

    print_stack_info(stack);

    return stack_assert(stack);
}





ProgramStatus stack_pop(Stack* stack  ON_DEBUG(___ int num_str_in_main))
{
    ON_DEBUG(stack->num_str_in_main = num_str_in_main;)
    ON_DEBUG(stack->name_current_func = "stack_pop";)


    stack->size -= 1;

    if (stack->size <= (stack->compacity / 4)) // <= так как вдруг нечет
    {
        stack->compacity /= 2; // на 2 ли?
        stack->arr = (StackElem_t*) realloc(stack->arr, stack->compacity * sizeof(StackElem_t)); // Функцию переписать!
    }

    print_stack_info(stack);

    return stack_assert(stack);
}


void print_error(ProgramStatus status)
{
    if      (status == STACK_NULL)
        PRINTF_RED("Ошибка: нулевой указатель на стек\n");

    else if (status == STACK_ARR_NULL)
        PRINTF_RED("Ошибка: нулевой указатель на массив стека\n");

    else if (status == SIZE_MORE_COMPASITY)
        PRINTF_RED("Ошибка: размер массива больше его максимального размера (скорее всего \"отрицательный\" size)\n");
    
    else if (status == TOO_LARGE_COMPASITY)
        PRINTF_RED("Ошибка: Массив превысил максимальный размер\n");
    
    else
        PRINTF_GREEN("Программа завершила работу без ошибок\n");
}
