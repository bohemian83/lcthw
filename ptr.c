#include <stdio.h>

int main() {

    int var = 10;
    int val[3] = {1, 2, 3};

    int * ptr = &var;

    int * ptr_v = &val[0];

    int sz = sizeof(ptr);

    printf("ptr content %p, var value %d, ptr address %p. size of ptr %d\n", ptr, var, &ptr, sz);
    printf("val is %p, val[0] address is %p, val[0] is %d\n", val, &val[0], val[0]);
    printf("ptr_v is %p, ptr_v[0] is %d, *(ptr_v+1)/ptr_v[1]/val[1] is %d, *(ptr_v+2)/ptr_v[2]/val[2] is %d\n", ptr_v, ptr_v[0], *(ptr_v+1), ptr_v[2]);
    return 0;
}