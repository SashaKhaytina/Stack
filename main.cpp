#include <stdio.h>
#include <assert.h>
#include <malloc.h>

const size_t COP_START = 16;

typedef double StackElem_t;

struct Stack
{
    StackElem_t* arr;
    size_t  size;
    size_t  compacity;
};

enum ProgramStatus {
    OK,
    STACK_NULL
};


// ProgramStatus stack_ctor(Stack* stack, StackElem_t* arr, size_t size, size_t compacity);
ProgramStatus default_stack_ctor(Stack* stack);
ProgramStatus stack_assert(Stack* stack);
void print_stack_info(Stack* stack);

ProgramStatus stack_dtor(Stack* stack);

ProgramStatus stack_push(Stack* stack, StackElem_t elem);
ProgramStatus stack_pop(Stack* stack);




int main()
{
    Stack stack = {};

    //StackElem_t a[] = {1, 2, 3};

    //stack_ctor(&stack, a, 3, 10);

    //printf("%p %p\n", &stack, stack.arr);

    //printf("%ld\n", stack.size);

    default_stack_ctor(&stack);

    stack_push(&stack, 5);
    stack_push(&stack, 1);
    stack_push(&stack, 3);

    stack_pop(&stack);
    stack_pop(&stack);
    stack_pop(&stack);
    //stack_pop(&stack); // Ха - ха compacity - 2, size - 18446744073709551615

    stack_push(&stack, 5);
    stack_push(&stack, 1);
    stack_push(&stack, 3);
    stack_push(&stack, 10);




    free(stack.arr);
}


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
    stack->size      = 0;
    stack->compacity = COP_START;
    stack->arr       = (StackElem_t*) calloc(COP_START, sizeof(StackElem_t));
       

    print_stack_info(stack);

    // Проверка на корректность
    return stack_assert(stack);
}

ProgramStatus stack_dtor(Stack* stack)
{
    free(stack->arr);
    stack->size = 0;      // надо ли это? (для красоты вроде только)
    stack->compacity = 0;

    return stack_assert(stack);

}



ProgramStatus stack_assert(Stack* stack)
{
    if (stack == NULL)
    {
        print_stack_info(stack);
        //assert(0);          // ну такое 
        return STACK_NULL;    // это лучше вроде
    }
    return OK;
}


void print_stack_info(Stack* stack)
{
    // printf("ТУТ ДОЛЖЕН БЫТЬ ВЫВОД СТЕКА");
    printf("compacity - %lu\n", stack->compacity);
    printf("size - %lu\n", stack->size);

    for (int i = (int) stack->size - 1; i >= 0; i--)
    {
        printf("arr[%d] = %f\n", i, stack->arr[i]);
    }

    printf("\n\n");

}


ProgramStatus stack_push(Stack* stack, StackElem_t elem)
{
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





ProgramStatus stack_pop(Stack* stack)
{
    stack->size -= 1;

    if (stack->size <= (stack->compacity / 4)) // <= так как вдруг нечет
    {
        stack->compacity /= 2; // на 2 ли?
        stack->arr = (StackElem_t*) realloc(stack->arr, stack->compacity * sizeof(StackElem_t)); // Функцию переписать!
    }

    print_stack_info(stack);

    return stack_assert(stack);
}
