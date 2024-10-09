#include <stdio.h>

#include "errors.h"

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
    
    case INCORRECT_POP:
        PRINTF_RED("Ошибка: Хотите попнуть, но в стеке ничего нет. Не много ли хотите???\n");
        break;
    
    default:
        PRINTF_GREEN("Программа завершила работу без ошибок\n");
        break;
    }
}