#include <stdio.h>
#include <math.h>

#include "errors.h"

void print_all_errors(Error_t status)
{
    if (status == 0)
    {
        PRINTF_GREEN("Программа завершила работу без ошибок\n");
        return;
    }

    int len = sizeof(Error_t) * 8;
    int bin_num[sizeof(Error_t) * 8] = {};
    
    for (int i = len - 1; i >= 0; i--) // TODO: можно проще
    {
        Error_t step_2 = (Error_t) pow(2, i);

        if (status >= step_2)
        {
            bin_num[i] = 1;
            status -= step_2;
        }
    }

    for (int i = 0; i < len; i++)
    {
        Error_t step_2 = (Error_t) pow(2, i);
        if (bin_num[i] == 1)
        {
            switch (step_2)
            {
            case STACK_NULL:
                PRINTF_RED("Ошибка: нулевой указатель на стек\n");
                break;
            
            case STACK_ARR_NULL:
                PRINTF_RED("Ошибка: нулевой указатель на массив стека\n");
                break;
            
            case SIZE_MORE_CAPACITY:
                PRINTF_RED("Ошибка: размер массива больше его максимального размера (скорее всего \"отрицательный\" size)\n");
                break;
            
            case TOO_LARGE_CAPACITY:
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
            
            case INCORRECT_POP:
                PRINTF_RED("Ошибка: Хотите попнуть, но в стеке ничего нет. Не много ли хотите???\n");
                break;
            
            default:
                break;
            }
        }
    }
}

