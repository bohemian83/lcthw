#include <stdio.h>
#include <ctype.h>
#include "dbg.h"

int print_a_message(const char *msg, size_t len)
{
    printf("A STRING:");
    for (int i = 0; i < len; i++) {
        printf("%c", msg[i]);
    }
    return 0;
}

int uppercase(const char *msg, size_t len)
{
    // BUG: \0 termination problems
    for (int i = 0; i < len; i++) {
        printf("%c", toupper(msg[i]));
    }

    printf("\n");

    return 0;
}

int lowercase(const char *msg, size_t len)
{
    // BUG: \0 termination problems
    for (int i = 0; i < len; i++) {
        printf("%c", tolower(msg[i]));
    }

    printf("\n");
    
    return 0;
}

int fail_on_purpose(const char *msg)
{
    return 1;
}