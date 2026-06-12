#ifndef DYNAMIC_ARRAY_H_
#define DYNAMIC_ARRAY_H_

#include <stdint.h>
#include "allocator.h"

/*
    Dynamic arrays should be defined as follows:
    DA_DEFINE_STRUCT(int_dynamic_array, int);
    A type struct int_dynamic_array will be available

    The macros can be used as follows:
    DA_INIT(&dynamic_array_object);
    Note: arrays should be provided BY POINTER (makes things cleaner because usually you will use the array pointer instead of the array)

    Requesting the implementation is done as follows:
    1. Make a source file dynamic_array.c
    2. Define DYNAMIC_ARRAY_IMPLEMENTATION at the top
    3. Below that include the header file
*/

typedef enum {
    DA_OK,
    DA_ALLOC_ERROR
} DA_Result;

#define DA_DEFINE_STRUCT(name, type) struct name { uint64_t size; uint64_t capacity; type * data; }

#ifndef DA_INIT_CAPACITY
#define DA_INIT_CAPACITY 64
#endif

typedef struct _da_template {
    uint64_t size;
    uint64_t capacity;
    void * data;
} _da_template;

void _da_init(_da_template * da);
void _da_deinit(_da_template * da, allocator_t * a);
DA_Result _da_expand(_da_template * da, size_t element_size, allocator_t * a);
DA_Result _da_pushback(_da_template * da, size_t element_size, void * element, allocator_t * a);

// void da_init(dynamic_array * me)
#define DA_INIT(dynamic_array) _da_init((_da_template *)(dynamic_array))

// DA_Result da_pushback(dynamic_array * me, element * e, allocator * a)
#define DA_PUSHBACK(dynamic_array, element, allocator) _da_pushback((_da_template *)(dynamic_array), sizeof((dynamic_array)->data[0]), (void *)(&element), allocator)

// void da_deinit(dynamic_array * me, allocator * a)
#define DA_DEINIT(dynamic_array, allocator) _da_deinit((_da_template *)(dynamic_array), allocator)

#endif

#ifdef DYNAMIC_ARRAY_IMPLEMENTATION
#undef DYNAMIC_ARRAY_IMPLEMENTATION

#include <string.h>

void _da_init(_da_template * da) {
    da->size = 0;
    da->capacity = 0;
    da->data = 0;
}

void _da_deinit(_da_template * da, allocator_t * a) {
    da->size = 0;
    da->capacity = 0;
    allocator_free(a, da->data);
}

DA_Result _da_expand(_da_template * da, size_t element_size, allocator_t * a) {
    size_t new_capacity = 0;
    if(0 == da->capacity) {
        new_capacity = DA_INIT_CAPACITY;
    } else {
        new_capacity = da->capacity * 2;
    }
    void * new_data = allocator_alloc(a, new_capacity * element_size);
    if(!new_data) return DA_ALLOC_ERROR;
    memcpy(new_data, da->data, da->capacity * element_size);
    allocator_free(a, da->data);
    da->data = new_data;
    da->capacity = new_capacity;
}

DA_Result _da_pushback(_da_template * da, size_t element_size, void * element, allocator_t * a) {
    DA_Result result = DA_OK;
    if(da->size == da->capacity) {
        result = _da_expand(da, element_size, a);
    }
    if(result != DA_OK) return result;
    void * base = ((char *)da->data) + da->size * element_size;
    memcpy(base, element, element_size);
    da->size += 1;
}

#endif