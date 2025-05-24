#define VECTOR_PREFIX vector_int_t
#define VECTOR_TYPE int
#include "mvector.h"
#include <stdio.h>


int main() {
    vector_int_t v = { 0 };
    vector_int_t_new(&v);

    for(int i = 0; i < 10; i++) {
        vector_int_t_pushback(&v, &i);
    }

    printf("Testing vector_int_t_get\n");
    for(int i = 0; i < 10; i++) {
        int result;
        vector_int_t_get(&v, i, &result);

        printf("%d\n", result);
    }

    printf("Testing vector_int_t_get_ref\n");
    for(int i = 0; i < 10; i++) {
        int * result;
        vector_int_t_get_ref(&v, i, &result);

        printf("%d\n", *result);
    }

    printf("Testing vector_int_t_popback\n");
    while(v.size) {
        int result;
        vector_int_t_popback(&v, &result);

        printf("%d\n", result);
    }

    return 0;
}