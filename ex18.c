#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

void die(const char *message)
{
    if (errno) {
        perror(message);
    } else {
        printf("ERROR: %s\n", message);
    }

    exit(1);
}

// a typedef creates a fake type, in this case for a function pointer
typedef int (*compare_cb) (int a, int b);

/**
 * A classic bubble sort function that uses the 
 * compare_cb to do the sorting. 
 */
int *bubble_sort(int *numbers, int count, compare_cb cmp)
{
    int temp = 0;
    int i = 0;
    int j = 0;
    int *target = malloc(count * sizeof(int));

    if (!target)
        die("Memory error.");

    memcpy(target, numbers, count * sizeof(int));

    for (i = 0; i < count; i++) {
        for (j = 0; j < count - 1; j++) {
            if (cmp(target[j], target[j + 1]) > 0) {
                temp = target[j + 1];
                target[j + 1] = target[j];
                target[j] = temp;
            }
        }
    }

    return target;
}

int sorted_order(int a, int b)
{
    return (a > b) - (a < b);
}

int reverse_order(int a, int b)
{
    return (a < b) - (a > b);
}

int strange_order(int a, int b)
{
    if (a == 0 || b == 0) {
        return 0;
    } else {
        return a % b;
    }
}

/**
 * Used to test that we are sorting things correctly
 * by doing the sort and printing it out
 */
void test_sorting(int *numbers, int count, compare_cb cmp)
{
    int i = 0; 
    int *sorted = bubble_sort(numbers, count, cmp);

    if (!sorted)
         die("Failed to sort as requested");

    for (i = 0; i < count; i++) {
        printf("%d ", sorted[i]);
    }
    printf("\n");

    free(sorted);
}

int main(int argc, char* argv[])
{
    if (argc < 2) die("USAGE: ./ex18 4 3 1 5 7");

    int count = argc - 1;
    int i = 0;
    char** inputs = argv + 1;

    int* numbers = malloc(count * sizeof(int));
    if (!numbers) die("Memory error.");
    
    // for (int i = 0; i < argc; i++) {
    //     for (int j = 0; j < 1; j++)
    //         printf("Character %d, %d: %c at address %p\n", i, j, (char)argv[i][j], &argv[i][j]);
    // }

    // printf("\nargv+1 %p\n*argv+1 %p\n**argv+1 %c\n&argv+1 %p\n", argv+1, *(argv+1), (char)**(argv+1), &argv+1);
    // printf("\nargv+2 %p\n*argv+2 %p\n**argv+2 %c\n&argv+2 %p\n", argv+2, *(argv+2), (char)**(argv+2), &argv+2);
    // printf("\nargv+3 %p\n*argv+3 %p\n**argv+3 %c\n&argv+3 %p\n\n", argv+3, *(argv+3), (char)**(argv+3), &argv+3);

    // char *str = "Hello";   // Pointer to the first character of a string
    // printf("%c\n\n", *str);   // %s prints the string until the null terminator

    for (i = 0; i < count; i++) {
        numbers[i] = atoi(inputs[i]);
    }

    test_sorting(numbers, count, sorted_order);
    test_sorting(numbers, count, reverse_order);
    test_sorting(numbers, count, strange_order);

    free(numbers);

    return 0;
}
