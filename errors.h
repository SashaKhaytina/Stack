#ifndef ERRORS
#define ERRORS

#define PRINTF_RED(string, ...)     printf("\x1b[31m" string "\x1b[0m", ##__VA_ARGS__)
#define PRINTF_GREEN(string, ...)   printf("\x1b[32m" string "\x1b[0m", ##__VA_ARGS__)

typedef unsigned long long Error_t;

enum ProgramStatus {
    OK = 0,
    STACK_NULL = 1 << 1, 
    STACK_ARR_NULL = 1 << 2,
    SIZE_MORE_CAPACITY = 1 << 3, 
    TOO_LARGE_CAPACITY = 1 << 4,
    NO_OPENING_FILE = 1 << 5,
    CHANGE_RIGHT_CANARY = 1 << 6, 
    CHANGE_LEFT_CANARY = 1 << 7, 
    HASH_STACK_ERROR = 1 << 8, 
    HASH_ARR_ERROR = 1 << 9, 
    CHANGE_LEFT_ARR_CANARY = 1 << 10,
    CHANGE_RIGHT_ARR_CANARY = 1 << 11,
    INCORRECT_POP = 1 << 12
};

void print_all_errors(Error_t status);


#endif