typedef struct {
    char name[50];
    int age;
} person_t;

#include <string.h>
#include <assert.h>
#include <stdio.h>

#define ALLOCATOR_HEAP_ALLOCATOR
#include "allocator.h"

#define MVECTOR_TYPE person_t
#define MVECTOR_IMPLEMENTATION
#include "mvector.h"

int main() {
    allocator_t a = { 0 };
    allocator_new_heap_allocator(&a);

    puts("----- TESTING 2-big-struct-on-stack.c -----");
    char * name = "abc";
    person_t p;
    strcpy(p.name, name);
    p.age = 50;

    vector_person_t vp;
    vector_person_t_new(&vp);

    for(int i = 0; i < 100; i++) {
        assert(vector_person_t_pushback(&vp, &p, &a) == i);
        assert(vector_person_t_error == ok);
    }

    assert(vector_person_t_get_ref(&vp, 0)->name != vector_person_t_get_ref(&vp, 1)->name);

    puts("----- TESTING 2-big-struct-on-stack.c DONE -----");
}
