#ifndef ERRORS
#define ERRORS

#define PRINTF_RED(string, ...)     printf("\x1b[31m" string "\x1b[0m", ##__VA_ARGS__)
#define PRINTF_GREEN(string, ...)   printf("\x1b[32m" string "\x1b[0m", ##__VA_ARGS__)

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
    CHANGE_RIGHT_ARR_CANARY,
    INCORRECT_POP
};

void print_error (ProgramStatus status);


#endif