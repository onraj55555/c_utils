#define ALLOCATOR_HEAP_ALLOCATOR
#include "allocator.h"

#define MDA_IMPLEMENTATION
#define MDA_TYPE int
#include "mda.h"

#include <assert.h>
#include <stdio.h>

int main() {
    allocator_t a = { 0 };
    allocator_new_heap_allocator(&a);

    puts("----- TESTING 1-int.c -----");
    da_int v;

    {
        da_int_new(&v);
        assert(da_int_error == ok);
        printf("--- NEW OK ---\n");
    }

    {
        for(int i = 0; i < 10; i++) {
            assert(da_int_pushback(&v, &i, &a) == i);
            assert(da_int_error == ok);
        }

        assert(v.size == 10);
        assert(v.capacity == 16);
        assert(da_int_error == ok);

        printf("--- PUSHBACK OK ---\n");
    }

    {
        int dest;
        for(int i = 9; i >= 0; i--) {
            int dest = da_int_popback(&v);
            assert(i == dest);
            assert(da_int_error == ok);
        }

        assert(v.size == 0);
        assert(v.capacity == 16);
        assert(da_int_error == ok);

        printf("--- POPBACK OK ---\n");
    }

    {
        for(int i = 0; i < 10; i++) {
            assert(da_int_pushback(&v, &i, &a) == i);
            assert(da_int_error == ok);
        }

        assert(v.size == 10);
        assert(v.capacity == 16);
        assert(da_int_error == ok);

        printf("--- PUSHBACK OK ---\n");
    }

    {
        da_int_delete(&v, &a);
        printf("--- DELETE OK ---\n");
    }

    puts("----- TESTING 1-int.c DONE -----");
}
