#define DEBUG

#ifdef DEBUG
#define ON_DEBUG(code) code

#else
#define ON_DEBUG(code)

#endif


#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>


#define PRINTF_RED(string, ...)     printf("\x1b[31m" string "\x1b[0m", ##__VA_ARGS__)
#define PRINTF_GREEN(string, ...)   printf("\x1b[32m" string "\x1b[0m", ##__VA_ARGS__)

#define IF_OK(status) if (!status) status = 
#define ___ ,  // ... НЕ ОЧЕНЬ ...

#define STACK_PUSH(stack, elem)  stack_push(stack, elem ON_DEBUG(___ __LINE__))
#define STACK_POP(stack)         stack_pop(stack ON_DEBUG(___ __LINE__))

typedef double StackElem_t;
typedef unsigned long long Canary_t;


const size_t COP_START = 16; // Это вообще норм?
const Canary_t CANARY = 0xDEFE0CE;
// const unsigned long long POISON = 0xAB0BA;

struct Stack
{
    ON_DEBUG(long long hash_stack;)  // при передаче указателя, надо передавать указатель на канарейку, а размер без sizeof(long long)
    ON_DEBUG(long long hash_arr;)

    Canary_t     left_canary =  CANARY;  // без дебага?
    StackElem_t* arr;
    size_t       size;
    size_t       compacity;

    ON_DEBUG(int num_str_in_main;)
    ON_DEBUG(char* name_current_func;)

    Canary_t     right_canary =  CANARY; // без дебага?
};

enum ProgramStatus {
    OK,
    STACK_NULL, 
    STACK_ARR_NULL,
    SIZE_MORE_COMPASITY, 
    TOO_LARGE_COMPASITY,
    NO_OPENING_FILE,
    CHANGE_RIGHT_CANARY, 
    CHANGE_LEFT_CANARY, 
    HASH_STACK_ERROR, 
    HASH_ARR_ERROR, 
    CHANGE_LEFT_ARR_CANARY,
    CHANGE_RIGHT_ARR_CANARY
};


// ProgramStatus stack_ctor(Stack* stack, StackElem_t* arr, size_t size, size_t compacity);

ProgramStatus default_stack_ctor   (Stack* stack);
ProgramStatus stack_assert         (Stack* stack);
void          print_stack_info     (Stack* stack);
void          file_print_stack_info(Stack* stack);
void          print_error          (ProgramStatus status);

ProgramStatus stack_dtor           (Stack* stack);

ProgramStatus stack_push           (Stack* stack, StackElem_t elem ON_DEBUG(___ int num_str_in_main));
ProgramStatus stack_pop            (Stack* stack  ON_DEBUG(___ int num_str_in_main));

long long     hash_func            (void* point, int size); // const void* data - ded wrote
// void*         poison_realloc       (void* point, size_t old_size, size_t new_size);


// ХЕШ, вывод в файл, Unitest, warnings, !!!!Вывод нескольких ошибок!!!!
// ЧТО ДЕЛАТЬ В ФАЙЛОМ ??? (ГДЕ ОТКРЫВАТЬ)

int main()
{
    Stack stack = {};

    ON_DEBUG(printf("DEBUG включен!\n\n");)


    ProgramStatus status = OK; // надо ли его оборачивать в ON_DEBUG?

    
    // ON_DEBUG(FILE* file = fopen("debug.txt", "w");)       // Плохо, потому что stack_print_info вызывается не только при DEBUG
    // ON_DEBUG(if (file == NULL) status = NO_OPENING_FILE;)


    IF_OK(status) default_stack_ctor(&stack);

    IF_OK(status) STACK_PUSH(&stack, 5);
    IF_OK(status) STACK_PUSH(&stack, 1);
    IF_OK(status) STACK_PUSH(&stack, 3);

    //stack.size--;
    //stack.left_canary = 0xDED;
    //stack.arr[0] = 9; 
    // *(stack.arr + stack.compacity)=100;

    IF_OK(status) STACK_POP(&stack);
    IF_OK(status) STACK_POP(&stack);
    IF_OK(status) STACK_POP(&stack);
    IF_OK(status) stack_pop(&stack ON_DEBUG(___ __LINE__)); // compacity - 2, size - 18446744073709551615 (надо проверять на адекватный размер и соответствие с compacity)

    IF_OK(status) STACK_PUSH(&stack, 5);
    IF_OK(status) STACK_PUSH(&stack, 1);
    IF_OK(status) STACK_PUSH(&stack, 3);
    IF_OK(status) STACK_PUSH(&stack, 10);


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
    // Здесь проверка на NULL
    ON_DEBUG(stack->name_current_func = "default_stack_ctor";)


    stack->size      = 0;
    stack->compacity = COP_START;
    stack->arr       = (StackElem_t*) calloc(COP_START, sizeof(StackElem_t));
    //stack->arr       = (StackElem_t*) poison_realloc(stack->arr, stack->size * sizeof(StackElem_t), COP_START * sizeof(StackElem_t));

    // stack->arr[0] = CANARY;
    // *(stack->arr + stack->compacity) = CANARY;


    // printf("%d - *stack\n", sizeof(Stack));
    // printf("%d - stack\n", (int*) stack);
    // printf("%d - long long\n", sizeof(long long));
    // printf("%d - hash func\n", hash_func(stack->arr, sizeof(StackElem_t) * (stack->size)));

    ON_DEBUG(stack->hash_stack = hash_func((void*) ((char*) stack + sizeof(long long)), sizeof(Stack) - 2 * sizeof(long long));)
    ON_DEBUG(stack->hash_arr = hash_func(stack->arr, (int) (sizeof(StackElem_t) * (stack->size)));)

    //print_stack_info(stack); // тут не надо

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

    if (stack->size >= stack->compacity) // размер больше максимального ??? = ??? почему dtor не
    {
        print_stack_info(stack);
        return SIZE_MORE_COMPASITY;
    }

    if (stack->compacity > 200000) // Надо ли делать проверку на большое size_t (подозрительно, не переполнение ли?)
    {
        print_stack_info(stack);
        return TOO_LARGE_COMPASITY;
    }

    ON_DEBUG
    (
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

    if (stack->hash_stack != hash_func(((void*)((char*) stack + sizeof(long long))), sizeof(Stack) - 2 * sizeof(long long))) // тут адекватная запись... она работает что ли?
    {
        printf("%lld - stack->hash_stack\n", stack->hash_stack);
        printf("%lld - счет\n", hash_func(((void*) ((char*)stack + sizeof(long long))), sizeof(Stack) - 2 * sizeof(long long)));
        print_stack_info(stack);
        return HASH_STACK_ERROR;
    }

    // // Оно может быть не создано!
    // if (stack->arr[0] != CANARY)
    // {
    //     print_stack_info(stack);
    //     return CHANGE_LEFT_ARR_CANARY;
    // }

    // if (*(stack->arr + stack->compacity) != CANARY)
    // {
    //     print_stack_info(stack);
    //     return CHANGE_RIGHT_ARR_CANARY;
    // }

    if (stack->hash_arr != hash_func(stack->arr, (int)(sizeof(StackElem_t) * (stack->size))))
    {
        print_stack_info(stack);
        return HASH_ARR_ERROR;
    }
    )

    return OK;
}


void print_stack_info(Stack* stack)  // это в define?.. 
{
    ON_DEBUG(printf("from main()::%d -- func - %s\n", stack->num_str_in_main, stack->name_current_func);)
    ON_DEBUG(printf("hash_stack - %lld\n", stack->hash_stack);)
    ON_DEBUG(printf("hash_arr - %lld\n", stack->hash_arr);)

    ON_DEBUG(printf("left canary - %llx\n", stack->left_canary);)
    ON_DEBUG(printf("right canary - %llx\n", stack->right_canary);)

    printf("compacity - %lu\n", stack->compacity);
    printf("size - %lu\n", stack->size);

    //ON_DEBUG(printf("left arr canary - %x\n", stack->arr[0]);)  //Почему если это раскомментить, то память утекает

    for (int i = (int) stack->size - 1; i >= 0; i--)
    {
        printf("arr[%d] = %f\n", i, stack->arr[i]);
    }

    //ON_DEBUG(printf("right arr canary - %x\n", *(stack->arr + stack->compacity));)

    printf("\n\n");

}

// треш
// void file_print_stack_info(Stack* stack)  // выводит dtor..........
// {
//     FILE* file = fopen("debug.txt", "w");  // норм ли это делать здесь? Ведь файл может не открыться, а это потенциальная ошибка
//     if (file == NULL)
//     {
//         printf("Файл для дебага не открылся");
//         return; // эм... но возвращать у этой функции ошибку - не очень.
//     }

//     ON_DEBUG(fprintf(file, "from main()::%d -- func - %s\n", stack->num_str_in_main, stack->name_current_func);)

//     fprintf(file, "left canary - %x\n", stack->left_canary);
//     fprintf(file, "right canary - %x\n", stack->right_canary);

//     fprintf(file, "compacity - %lu\n", stack->compacity);
//     fprintf(file, "size - %lu\n", stack->size);

//     for (int i = (int) stack->size; i >= 1; i--)
//     {
//         fprintf(file, "arr[%d] = %f\n", i, stack->arr[i]);
//     }

//     fprintf(file, "\n\n");

//     fclose(file); // всегда остается последняя запись

// }



ProgramStatus stack_push(Stack* stack, StackElem_t elem ON_DEBUG(___ int num_str_in_main))
{
    ProgramStatus stat = stack_assert(stack);
    if (stat) return stat;

    ON_DEBUG(stack->num_str_in_main = num_str_in_main;)
    ON_DEBUG(stack->name_current_func = "stack_push";)

    stack->arr[stack->size] = elem;
    stack->size += 1;

    if (stack->size == stack->compacity)
    {
        stack->compacity *= 2;
        stack->arr = (StackElem_t*) realloc(stack->arr, (stack->compacity) * sizeof(StackElem_t)); // Функцию переписать!
    }
    
    ON_DEBUG(stack->hash_arr = hash_func(stack->arr,(int) (sizeof(StackElem_t) * (stack->size)));)
    ON_DEBUG(stack->hash_stack = hash_func((void*) ((char*)stack + sizeof(long long)), sizeof(Stack) - 2*sizeof(long long));)

    print_stack_info(stack); // здесь не надо

    return stack_assert(stack);
}





ProgramStatus stack_pop(Stack* stack  ON_DEBUG(___ int num_str_in_main))
{
    ProgramStatus stat = stack_assert(stack);
    if (stat) return stat;

    ON_DEBUG(stack->num_str_in_main = num_str_in_main;)
    ON_DEBUG(stack->name_current_func = "stack_pop";)


    stack->size -= 1;

    if (stack->size <= (stack->compacity / 4)) // <= так как вдруг нечет
    {
        stack->compacity /= 2; // на 2 ли?
        stack->arr = (StackElem_t*) realloc(stack->arr, (stack->compacity) * sizeof(StackElem_t)); // Функцию переписать!

    }

    ON_DEBUG(stack->hash_arr = hash_func(stack->arr, (int)(sizeof(StackElem_t) * (stack->size)));) // ЭТО ДОЛЖНО БЫТЬ РАНЬШЕ!!! НО ПОЧЕМУ???
    ON_DEBUG(stack->hash_stack = hash_func((void*) ((char*)stack + sizeof(long long)), sizeof(Stack) - 2*sizeof(long long));)

    print_stack_info(stack); // здесь не надо

    return stack_assert(stack);
}


void print_error(ProgramStatus status)
{
    switch (status)
    {
    case STACK_NULL:
        PRINTF_RED("Ошибка: нулевой указатель на стек\n");
        break;
    
    case STACK_ARR_NULL:
        PRINTF_RED("Ошибка: нулевой указатель на массив стека\n");
        break;
    
    case SIZE_MORE_COMPASITY:
        PRINTF_RED("Ошибка: размер массива больше его максимального размера (скорее всего \"отрицательный\" size)\n");
        break;
    
    case TOO_LARGE_COMPASITY:
        PRINTF_RED("Ошибка: Массив превысил максимальный размер\n");
        break;
    
    case NO_OPENING_FILE:
        PRINTF_RED("Ошибка: Ошибка открытия файла\n");
        break;
    
    case CHANGE_LEFT_CANARY:
        PRINTF_RED("Ошибка: Левая канарейка сдохла\n");
        break;
    
    case CHANGE_RIGHT_CANARY:
        PRINTF_RED("Ошибка: Правая канарейка сдохла\n");
        break;
    
    case HASH_STACK_ERROR:
        PRINTF_RED("Ошибка: Ошибка хэша стека\n");
        break;
    
    case CHANGE_LEFT_ARR_CANARY:
        PRINTF_RED("Ошибка: Левая канарейка в массиве сдохла\n");
        break;
    
    case CHANGE_RIGHT_ARR_CANARY:
        PRINTF_RED("Ошибка: Правая канарейка в массиве сдохла\n");
        break;

    case HASH_ARR_ERROR:
        PRINTF_RED("Ошибка: Ошибка хэша массива\n");
        break;
    
    default:
        PRINTF_GREEN("Программа завершила работу без ошибок\n");
        break;
    }
}


long long hash_func(void* point, int size)
{
    int const1 = 13;
    int const2 = 41;

    long long hash = 0;
    point = (char*) point;

    for (int i = 0; i < size; i++)
    {
        hash = (hash * const1 + (long long) *((char*)point + i)) % const2;
        //printf("%lld - HASHHHHHH\n", hash);
    }

    return hash;
}


// void* poison_realloc(void* point, size_t old_size, size_t new_size)
// {
//     void* new_point = memcpy(realloc(point, new_size + 1), point, old_size);     // Копирует
//     memset((void*) ((char*) new_point + old_size), POISON, new_size - old_size); // Заполняет

//     return new_point;
// }
