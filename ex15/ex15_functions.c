#include <stdio.h>

// Function prototypes
void print_using_indexing(char **names, int *ages, int count);
void print_using_pointers(char **names, int *ages, int count);
void print_pointers_as_arrays(char **names, int *ages, int count);
void print_complex_way(char **names, int *ages, int count);
void print_addresses(char **names, int *ages, int count);

int main(int argc, char *argv[])
{
    // create two arrays we care about
    int ages[] = { 23, 43, 12, 89, 2 };
    char *names[] = {
        "Alan", "Frank",
        "Mary", "John", "Lisa"
    };

    // safely get the size of ages
    int count = sizeof(ages) / sizeof(int);

    // Call functions for each printing method
    printf("First way: Using indexing\n");
    print_using_indexing(names, ages, count);

    printf("---\nSecond way: Using pointers\n");
    print_using_pointers(names, ages, count);

    printf("---\nThird way: Pointers as arrays\n");
    print_pointers_as_arrays(names, ages, count);

    printf("---\nFourth way: Complex pointer manipulation\n");
    print_complex_way(names, ages, count);

    printf("---\nFifth way: Printing addresses\n");
    print_addresses(names, ages, count);

    return 0;
}

// First way using indexing
void print_using_indexing(char **names, int *ages, int count) {
    for (int i = 0; i < count; i++) {
        printf("%s has %d years alive.\n", names[i], ages[i]);
    }
}

// Second way using pointers
void print_using_pointers(char **names, int *ages, int count) {
    for (int i = 0; i < count; i++) {
        printf("%s is %d years old.\n", *(names + i), *(ages + i));
    }
}

// Third way where pointers are just arrays
void print_pointers_as_arrays(char **names, int *ages, int count) {
    for (int i = 0; i < count; i++) {
        printf("%s is %d years old again.\n", names[i], ages[i]);
    }
}

// Fourth way: complex pointer manipulation
void print_complex_way(char **names, int *ages, int count) {
    char **cur_name = names;
    int *cur_age = ages;
    for (; (cur_age - ages) < count; cur_name++, cur_age++) {
        printf("%s lived %d years so far.\n", *cur_name, *cur_age);
    }
}

// Fifth way: printing the addresses used by the pointers
void print_addresses(char **names, int *ages, int count) {
    for (int i = 0; i < count; i++) {
        printf("Address of %s: %p, Address of %d: %p\n",
               names[i], (void*)&names[i], ages[i], (void*)&ages[i]);
    }
}
