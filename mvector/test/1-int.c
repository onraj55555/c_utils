#define ALLOCATOR_HEAP_ALLOCATOR
#include "allocator.h"

#define MVECTOR_IMPLEMENTATION
#define MVECTOR_TYPE int
#include "mvector.h"

#include <assert.h>
#include <stdio.h>

int main() {
    allocator_t a = { 0 };
    allocator_new_heap_allocator(&a);

    puts("----- TESTING 1-int.c -----");
    vector_int v;

    {
        vector_int_new(&v);
        assert(vector_int_error == ok);
        printf("--- NEW OK ---\n");
    }

    {
        for(int i = 0; i < 10; i++) {
            assert(vector_int_pushback(&v, &i, &a) == i);
            assert(vector_int_error == ok);
        }

        assert(v.size == 10);
        assert(v.capacity == 16);
        assert(vector_int_error == ok);

        printf("--- PUSHBACK OK ---\n");
    }

    {
        int dest;
        for(int i = 9; i >= 0; i--) {
            vector_int_popback(&v, &dest);
            assert(i == dest);
            assert(vector_int_error == ok);
        }

        assert(v.size == 0);
        assert(v.capacity == 16);
        assert(vector_int_error == ok);

        printf("--- POPBACK OK ---\n");
    }

    {
        for(int i = 0; i < 10; i++) {
            assert(vector_int_pushback(&v, &i, &a) == i);
            assert(vector_int_error == ok);
        }

        assert(v.size == 10);
        assert(v.capacity == 16);
        assert(vector_int_error == ok);

        printf("--- PUSHBACK OK ---\n");
    }

    {
        vector_int_delete(&v, &a);
        printf("--- DELETE OK ---\n");
    }

    puts("----- TESTING 1-int.c DONE -----");
}
