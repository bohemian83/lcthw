#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[]){

    char **inputs = argv + 1;

    printf("%p\n", *argv);
    printf("%d, %p, %p, %p\n", *(*inputs), *inputs, inputs, *(argv+1));

    for (int i = 1; i < argc; i++) {
        for (int j = 0; j < 3; j++)
            printf("Character %d, %d: %c at address %p\n", i, j, (char)argv[i][j], &argv[i][j]);
    }

    return 0;
}