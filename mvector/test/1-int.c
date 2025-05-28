#define MVECTOR_TYPE int
#include "../mvector.h"
#define MVECTOR_IMPL
#include "../mvector.h"

#include <assert.h>
#include <stdio.h>

int main() {
    vector_int_t v;

    {
        vector_int_new(&v);
        assert(vector_int_error == ok);
        printf("--- NEW OK ---\n");
    }

    {
        for(int i = 0; i < 10; i++) {
            assert(vector_int_pushback(&v, &i) == i);
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
            assert(vector_int_pushback(&v, &i) == i);
            assert(vector_int_error == ok);
        }

        assert(v.size == 10);
        assert(v.capacity == 16);
        assert(vector_int_error == ok);

        printf("--- PUSHBACK OK ---\n");
    }

    {
        vector_int_delete(&v);
        printf("--- DELETE OK ---\n");
    }
}