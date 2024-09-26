#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct Person {
    char *name;
    int age;
    int height;
    int weight;
};

struct Person Person_create(char *name, int age, int height, int weight)
{
    struct Person who;

    who.name = strdup(name);
    who.age = age;
    who.height = height;
    who.weight = weight;

    return who;
}

void Person_print(struct Person who)
{
    printf("Name: %s\n", who.name);
    printf("\tAge: %d\n", who.age);
    printf("\tHeight: %d\n", who.age);
    printf("\tWeight: %d\n", who.weight);
}

int main(int argc, char *argv[])
{
    //make two people structures
    struct Person joe = Person_create("Joe", 32, 64, 140);

    //print them out and where they are in memory
    // printf("Joe is at memory location %p:\n", joe);
    Person_print(joe);

    //make everyone age 20 years aad print them again
    joe.age += 20;
    joe.height -= 2;
    joe.weight += 40;
    Person_print(joe);

    return 0;
}