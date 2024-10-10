#ifndef ERRORS
#define ERRORS

#define PRINTF_RED(string, ...)     printf("\x1b[31m" string "\x1b[0m", ##__VA_ARGS__)
#define PRINTF_GREEN(string, ...)   printf("\x1b[32m" string "\x1b[0m", ##__VA_ARGS__)

typedef unsigned long long Error_t;

enum ProgramStatus {
    OK,
    STACK_NULL = 2, 
    STACK_ARR_NULL = 4,
    SIZE_MORE_COMPASITY = 8, 
    TOO_LARGE_COMPASITY = 16,
    NO_OPENING_FILE = 32,
    CHANGE_RIGHT_CANARY = 64, 
    CHANGE_LEFT_CANARY = 128, 
    HASH_STACK_ERROR = 256, 
    HASH_ARR_ERROR = 512, 
    CHANGE_LEFT_ARR_CANARY = 1024,
    CHANGE_RIGHT_ARR_CANARY = 2048,
    INCORRECT_POP = 4096
};

void print_all_errors(Error_t status);
// void print_error (ProgramStatus status);


#endif