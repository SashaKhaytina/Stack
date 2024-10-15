#ifndef STACK_COMMANDS
#define STACK_COMMANDS


#ifdef DEBUG
#define ON_DEBUG(...) __VA_ARGS__
#define CHECK(status) if (status){stack_assert(&stack); print_all_errors(status); exit((int) status);}
#define PRINT_STACK

#define CHECK_STACK_INFO Error_t status = stack_assert(stack);\
                         if (status) return status;
#else
#define ON_DEBUG(...)
#define CHECK(status)
#define CHECK_STACK_INFO
#define PRINT_STACK\
    for (int i = (int) stack.size - 1; i >= 0; i--)\
    {\
        printf("arr[%d] = %f\n", i, stack.arr[i]);\
    }\
    printf("\n");

#endif


#include <stdio.h>

#include "utils.h"
#include "errors.h"

#define STACK_PUSH(stack, elem)  stack_push(stack, elem ON_DEBUG(, __LINE__))
#define STACK_POP(stack)         stack_pop(stack ON_DEBUG(, __LINE__))


typedef double             StackElem_t;
typedef unsigned long long Canary_t;

const Canary_t CANARY = 0xDEFE0CE;
const StackElem_t CANARY_ARR = 9999;
const StackElem_t POISON = 1234;
const size_t MAX_CAPACITY = 200000;

struct Stack
{
    ON_DEBUG(Hash_t   hash_stack;)
    ON_DEBUG(Hash_t   hash_arr;)
    ON_DEBUG(Canary_t left_canary =  CANARY;)

    StackElem_t* arr;
    size_t       size;
    size_t       capacity;

    ON_DEBUG(int         code_num_string;)
    ON_DEBUG(const char* name_current_func;)
    ON_DEBUG(Canary_t    right_canary = CANARY;)
};


Error_t default_stack_ctor(Stack* stack, size_t capacity);
Error_t stack_dtor        (Stack* stack);
Error_t stack_push        (Stack* stack, StackElem_t elem ON_DEBUG(, int code_num_string));
Error_t stack_pop         (Stack* stack  ON_DEBUG(, int code_num_string));
Error_t stack_assert      (Stack* stack);
void    print_stack_info  (Stack* stack, Error_t status);
void    stack_realloc     (Stack* stack, size_t new_size);
void    stack_memset      (StackElem_t* point, size_t old_size, size_t new_size);




#endif