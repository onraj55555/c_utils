#ifndef MVECTOR_H_
#define MVECTOR_H_
#include <stdint.h>

#define MVECTOR_CONCAT2(a, b) a##b
#define MVECTOR_CONCAT(a, b) MVECTOR_CONCAT2(a, b)

#endif

typedef struct VECTOR_PREFIX VECTOR_PREFIX;

struct VECTOR_PREFIX {
    VECTOR_TYPE * data;
    uint64_t size;
    uint64_t capacity;
};

int MVECTOR_CONCAT(VECTOR_PREFIX, _new)(VECTOR_PREFIX * v);
int MVECTOR_CONCAT(VECTOR_PREFIX, _pushback)(VECTOR_PREFIX * v, VECTOR_TYPE * t);
int MVECTOR_CONCAT(VECTOR_PREFIX, _popback)(VECTOR_PREFIX * v, VECTOR_TYPE * t);
int MVECTOR_CONCAT(VECTOR_PREFIX, _get)(VECTOR_PREFIX * v, uint64_t i, VECTOR_TYPE * result);
int MVECTOR_CONCAT(VECTOR_PREFIX, _get_ref)(VECTOR_PREFIX * v, uint64_t i, VECTOR_TYPE ** result);

#ifdef MVECTOR_IMPL

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// int VECTOR_PREFIX_new(VECTOR_PREFIX * v);
int MVECTOR_CONCAT(VECTOR_PREFIX, _new)(VECTOR_PREFIX * v) {
    v->data = 0;
    v->size = 0;
    v->capacity = 0;

    return 1;
}

// int VECTOR_PREFIX_pushback(VECTOR_PREFIX * v, VECTOR_TYPE * t);
int MVECTOR_CONCAT(VECTOR_PREFIX, _pushback)(VECTOR_PREFIX * v, VECTOR_TYPE * t) {
    // If vector is still of size 0, initialise it with value 2
    if(v->capacity == 0) {
        v->data = malloc(sizeof(VECTOR_TYPE) * 2);
        if(v->data == 0) return -1;
        v->capacity = 2;
    }
    else if(v->size == v->capacity) {
        VECTOR_TYPE * temp = malloc(sizeof(VECTOR_TYPE) * v->capacity * 2);
        if(temp == 0) return -1;

        memcpy(temp, v->data, sizeof(VECTOR_TYPE) * v->size);
    }

    memcpy(v->data + v->size++, t, sizeof(VECTOR_TYPE));

    return 1;
}

int MVECTOR_CONCAT(VECTOR_PREFIX, _popback)(VECTOR_PREFIX * v, VECTOR_TYPE * t) {
    if(v->size == 0) return 1;

    memcpy(t, v->data + --v->size, sizeof(VECTOR_TYPE));

    return 1;
}

// PRIVATE
// int VECTOR_PREFIX_check_out_of_bounds(VECTOR_PREFIX * v, uint64_t i);
int MVECTOR_CONCAT(VECTOR_PREFIX, _check_out_of_bounds)(VECTOR_PREFIX * v, uint64_t i) {
    return i < 0 || i >= v->size;
}

// int VECTOR_PREFIX_get(VECTOR_PREFIX * v, uint64_t i, VECTOR_TYPE * result);
int MVECTOR_CONCAT(VECTOR_PREFIX, _get)(VECTOR_PREFIX * v, uint64_t i, VECTOR_TYPE * result) {
    if(vector_int_t_check_out_of_bounds(v, i)) return -1;

    memcpy(result, v->data + i, sizeof(VECTOR_TYPE));

    return 1;
}

// int VECTOR_PREFIX_get_ref(VECTOR_PREFIX * v, uint64_t i, VECTOR_TYPE ** result);
int MVECTOR_CONCAT(VECTOR_PREFIX, _get_ref)(VECTOR_PREFIX * v, uint64_t i, VECTOR_TYPE ** result) {
    if(vector_int_t_check_out_of_bounds(v, i)) return -1;

    *result = v->data + i;

    return 1;
}
#endif