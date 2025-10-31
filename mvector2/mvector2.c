#include "../types/types.h"
#include <stdlib.h>

void panic() {
    exit(255);
}

typedef struct {
    void * data;
    u64 size;
    u64 capacity;
} _vector;

void _vector_double(_vector * v, u64 mem_size) {
    if(v->capacity == 0) {
        v->data = malloc(2 * mem_size);
        if(!v->data) panic();
        v->capacity = 2;
        return;
    }

    void * t = realloc()
}

#define vector_double(v) if(v.capacity == 0) { v.data = calloc(2, sizeof(*v.data)); if(!v.data) { panic(); } v.capacity = 2; } else { void * t = calloc(2 * v.capacity)}

typedef struct {
    int * data;
    u64 size;
    u64 capacity;
} vector_int;


int main() {

}