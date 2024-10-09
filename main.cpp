#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>

#include "stack_commands.h"
#include "errors.h"
#include "utils.h"


const size_t INITIAL_CAPACITY = 16; 

// ХЕШ, вывод в файл, Unitest, warnings, !!!!Вывод нескольких ошибок!!!!
// ЧТО ДЕЛАТЬ В ФАЙЛОМ ??? (ГДЕ ОТКРЫВАТЬ)

int main()
{
    Stack stack = {};

    ON_DEBUG(printf("DEBUG включен!\n\n");)


    ProgramStatus status = OK; 

    status = default_stack_ctor(&stack, INITIAL_CAPACITY);
    CHECK(status);

    // status = STACK_POP(&stack);
    // CHECK(status);

    status = STACK_PUSH(&stack, 1);
    CHECK(status);

    //stack.right_canary = 0xDEDEAD;

    status = STACK_PUSH(&stack, 3);
    CHECK(status);

    //stack.size--;
    //stack.left_canary = 0xDED;
    //stack.arr[-1] = 9; 
    //stack.arr[stack.capacity] = 9; 
    //*(stack.arr + stack.capacity)=100;

    //stack.arr[2] = 90; // эта ошибка не ловится... Уже ловится!


    status = STACK_PUSH(&stack, 3);
    CHECK(status);

    status = STACK_POP(&stack);
    CHECK(status);

    status = STACK_POP(&stack);
    CHECK(status);

    status = STACK_PUSH(&stack, 5);
    CHECK(status);

    status = STACK_PUSH(&stack, 1);
    CHECK(status);

    status = STACK_PUSH(&stack, 3);
    CHECK(status);
    
    status = STACK_PUSH(&stack, 10);
    CHECK(status);

    stack_dtor(&stack);

    print_error(status); // Тут "крах системы" (цитата), если ранее не закаллочен
}
