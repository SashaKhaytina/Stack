#define DEBUG

#ifdef DEBUG
#define ON_DEBUG(...) __VA_ARGS__

#else
#define ON_DEBUG(code)

#endif


#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>

#define PRINTF_RED(string, ...)     printf("\x1b[31m" string "\x1b[0m", ##__VA_ARGS__)
#define PRINTF_GREEN(string, ...)   printf("\x1b[32m" string "\x1b[0m", ##__VA_ARGS__)

//#define IF_OK(status) if (!status) status = 

#define CHECK(status) if (status){stack_assert(&stack); print_error(status); exit(status);}

#define STACK_PUSH(stack, elem)  stack_push(stack, elem ON_DEBUG(, __LINE__))
#define STACK_POP(stack)         stack_pop(stack ON_DEBUG(, __LINE__))

typedef double StackElem_t; // stack_elem_t
typedef unsigned long long Canary_t; // canary_t
typedef long long Hash_t;


const size_t INITIAL_CAPACITY = 16; // Это вообще норм?

const Canary_t CANARY = 0xDEFE0CE;
const StackElem_t CANARY_ARR = 9999;
//const unsigned long long POISON = 0xAB0BA;
const StackElem_t POISON = 7;

struct Stack
{
    ON_DEBUG(Hash_t   hash_stack;)  // при передаче указателя, надо передавать указатель на канарейку, а размер без sizeof(long long)
    ON_DEBUG(Hash_t   hash_arr;)
    ON_DEBUG(Canary_t    left_canary =  CANARY;)

    StackElem_t* arr;
    size_t       size;
    size_t       capacity;

    ON_DEBUG(int         code_num_string;)
    ON_DEBUG(const char* name_current_func;)
    ON_DEBUG(Canary_t    right_canary = CANARY;)
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


// ProgramStatus stack_ctor(Stack* stack, StackElem_t* arr, size_t size, size_t capacity);

ProgramStatus default_stack_ctor   (Stack* stack, size_t capacity);
ProgramStatus stack_assert         (Stack* stack);
void          print_stack_info     (Stack* stack);
void          file_print_stack_info(Stack* stack);
void          print_error          (ProgramStatus status);

ProgramStatus stack_dtor           (Stack* stack);

ProgramStatus stack_push           (Stack* stack, StackElem_t elem ON_DEBUG(, int code_num_string));
ProgramStatus stack_pop            (Stack* stack  ON_DEBUG(, int code_num_string));

Hash_t        hash_func            (void* point, int size); // const void* data - ded wrote
void*         poison_realloc       (void* point, size_t old_size, size_t new_size);


// ХЕШ, вывод в файл, Unitest, warnings, !!!!Вывод нескольких ошибок!!!!
// ЧТО ДЕЛАТЬ В ФАЙЛОМ ??? (ГДЕ ОТКРЫВАТЬ)

int main()
{
    Stack stack = {};

    ON_DEBUG(printf("DEBUG включен!\n\n");)


    ProgramStatus status = OK; // надо ли его оборачивать в ON_DEBUG?

    status = default_stack_ctor(&stack, INITIAL_CAPACITY);
    CHECK(status);

    //IF_OK(status) default_stack_ctor(&stack);

    // status = default_stack_ctor(&stack); // TOD: change IF_OK to that, and it has to be none if debug was not defined
    // CHECK(status);

    //ON_DEBUG(CHECK(status) status =) STACK_PUSH(&stack, 5);
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

    //IF_OK(status) STACK_POP(&stack);
    //IF_OK(status) stack_pop(&stack ON_DEBUG(, __LINE__)); // capacity - 2, size - 18446744073709551615 (надо проверять на адекватный размер и соответствие с capacity)

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


ProgramStatus default_stack_ctor(Stack* stack, size_t capacity) // TOD: you can pass capacity as function argument, but its not necessary
{
    // Здесь проверка на NULL
    ON_DEBUG(stack->name_current_func = __PRETTY_FUNCTION__;)


    stack->size     = 0;
    stack->capacity = capacity;
    //stack->arr      = (StackElem_t*) (calloc(capacity ON_DEBUG(+ 2), sizeof(StackElem_t)));
    //char* a = (char*) calloc(capacity ON_DEBUG(+ 2), sizeof(StackElem_t));

    stack->arr      = (StackElem_t*) (((char*) poison_realloc(stack->arr, 0, (capacity ON_DEBUG(+ 2)) * sizeof(StackElem_t))) + sizeof(StackElem_t)); // Ёпсель-мопсель...

    printf("%f - 111111\n", stack->arr[-1]);
    ON_DEBUG(stack->arr[-1] = CANARY_ARR;) // TOD: use canary only in debug mode
    printf("%f - 222222\n", stack->arr[-1]);

    //print_stack_info(stack);

    //ON_DEBUG(stack->arr[0] = CANARY_ARR;)
    //ON_DEBUG(*(stack->arr + stack->capacity) = CANARY;) 
//    printf("%f - 111111\n", stack->arr[stack->capacity]);
    ON_DEBUG(stack->arr[stack->capacity] = CANARY_ARR;)


    // printf("%d - *stack\n", sizeof(Stack));
    // printf("%d - stack\n", (int*) stack);
    // printf("%d - long long\n", sizeof(long long));
    // printf("%d - hash func\n", hash_func(stack->arr, sizeof(StackElem_t) * (stack->size)));

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
    stack->size = 0;      // надо ли это? (для красоты вроде только)
    stack->capacity = 1;

    return OK; // И у него всегда все ок! :)  ..???

}


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
    ProgramStatus stat = stack_assert(stack); // TOD: all asserts are disabled when DEBUG was not defined, do that
    if (stat) return stat;
    )

    ON_DEBUG(stack->code_num_string = code_num_string;)
    ON_DEBUG(stack->name_current_func = __PRETTY_FUNCTION__;) // TOD: use macro __PRETTY_FUNCTION__

    if (stack->size == stack->capacity) // TOD: do that later, before pushing element // WHY - 1? (Без этого улетает с длинной массива) (не актуально вроде. Надо просто нормально делать вывод)
    {
        //stack->arr = (StackElem_t*) poison_realloc((char*) stack->arr - sizeof(StackElem_t), stack->capacity, stack->capacity * 2 * sizeof(StackElem_t)); // Функцию переписать!

        stack->arr      = (StackElem_t*) (((char*) poison_realloc((StackElem_t*) ((char*)stack->arr - sizeof(StackElem_t)), (stack->capacity ON_DEBUG(+ 2)) * sizeof(StackElem_t), (2 * stack->capacity ON_DEBUG(+ 2)) * sizeof(StackElem_t))) + sizeof(StackElem_t)); // Ёпсель-мопсель...

        stack->capacity *= 2;
        // TODO: do recalloc function
        ON_DEBUG(stack->arr[stack->capacity] = CANARY_ARR;) // TOD: also on debug mode
    }
 
    // TODO: проверь логику для size
    //stack->size += 1; // TOD: когда ты заходишь в функцию, size должен уже быть там, куда ты должна пушить новый элемент
    stack->arr[stack->size] = elem;
    stack->size += 1; // TOD: правильно увеличивать size вот здесь, а не раньше
    
    //ON_DEBUG(stack->hash_arr = hash_func(stack->arr, sizeof(StackElem_t) * (stack->size));)
    ON_DEBUG(stack->hash_arr = hash_func(stack->arr, sizeof(StackElem_t) * (stack->capacity));)

    ON_DEBUG(stack->hash_stack = hash_func((void*) stack + sizeof(Hash_t), sizeof(Stack) - sizeof(Hash_t));)

    print_stack_info(stack); // здесь не надо

    return stack_assert(stack);
}





ProgramStatus stack_pop(Stack* stack  ON_DEBUG(, int code_num_string))
{
    ProgramStatus stat = stack_assert(stack);
    if (stat) return stat;

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

         ON_DEBUG(stack->arr[stack->capacity] = CANARY_ARR;) // TOD: alse on debug mode
    }

    stack->arr[stack->size] = POISON; // тут может быть ошибка!!!

    //ON_DEBUG(stack->hash_arr = hash_func(stack->arr, sizeof(StackElem_t) * (stack->size));) // ЭТО ДОЛЖНО БЫТЬ РАНЬШЕ!!! НО ПОЧЕМУ???
    ON_DEBUG(stack->hash_arr = hash_func(stack->arr, sizeof(StackElem_t) * (stack->capacity));)
    ON_DEBUG(stack->hash_stack = hash_func((void*) stack + sizeof(Hash_t), sizeof(Stack) - sizeof(Hash_t));)

    
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
        PRINTF_RED("Ошибка: Левая канарейка помЭрла\n");
        break;
    
    case CHANGE_RIGHT_CANARY:
        PRINTF_RED("Ошибка: Правая канарейка помЭрла\n");
        break;
    
    case HASH_STACK_ERROR:
        PRINTF_RED("Ошибка: Ошибка хэша стека\n");
        break;
    
    case CHANGE_LEFT_ARR_CANARY:
        PRINTF_RED("Ошибка: Левая канарейка в массиве помЭрла\n");
        break;
    
    case CHANGE_RIGHT_ARR_CANARY:
        PRINTF_RED("Ошибка: Правая канарейка в массиве помЭрла\n");
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


void* poison_realloc(void* point, size_t old_size, size_t new_size) // TODO: it doesn't work correctly because old_size can be bigger than new_size, check this
{
    printf("%p - point\n", point);
    // void* new_point = memcpy(realloc(point, new_size + 1), point, old_size);     // Копирует
    if (old_size < new_size)
    {
        printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAaa\n");
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
