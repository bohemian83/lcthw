#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "dbg.h"

int main(int argc, char *argv[])
{
    FILE *file = fopen("logfind", "r");
    check(file, "Error opening file.");
    
    fclose(file);
    return 0;

error:
    return -1;
}