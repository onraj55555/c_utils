#ifndef HASHMAP_OPEN_H_
#define HASHMAP_OPEN_H_

#include <stdio.h>
#include <stdint.h>
#include "allocator.h"
#include <stdbool.h>
#include <string.h>

#ifndef HMO_LOAD_FACTOR
#define HMO_LOAD_FACTOR 0.7
#endif

#ifndef HMO_INIT_CAPACITY
#define HMO_INIT_CAPACITY 64
#endif

#define HMO_LINEAR_PROBING

typedef enum {
    HMO_OK,
    HMO_ALLOC_ERROR,
    HMO_KEY_ALREADY_EXISTS_ERROR
} HMO_Result;

// TODO: these functions all need the hmo prefix to reduce name collisions
typedef void (*key_free_func)(void * key, allocator_t * a);
typedef void (*value_free_func)(void * value, allocator_t * a);
typedef uint64_t (*hash_func)(void * key);
typedef bool (*key_cmp_func)(void * key1, void * key2);

typedef struct hmo_t {
    size_t size;
    size_t capacity;
    size_t key_size;
    size_t value_size;
    size_t key_offset;
    size_t value_offset;
    size_t kv_size;
    hash_func hash_function;
    key_cmp_func key_cmp_function;
    key_free_func key_free_function;
    value_free_func value_free_function;
    uint8_t * data;
} hmo_t;

void _hmo_init(hmo_t *hmo, size_t key_size, size_t key_alignment, size_t value_size, size_t value_alignment, hash_func hash_function, key_cmp_func key_cmp_function);
void hmo_deinit(hmo_t * hmo, allocator_t * a);
void hmo_set_key_free_function(hmo_t * hmo, key_free_func key_free_function);
void hmo_set_value_free_function(hmo_t * hmo, value_free_func value_free_function);
HMO_Result hmo_insert(hmo_t * hmo, void * key, void * value, allocator_t * a);
HMO_Result _hmo_remove(hmo_t * hmo, uint64_t kv_struct_size, void * key);
bool hmo_contains_key(hmo_t * hmo, void * key);
void * hmo_get_value_ref(hmo_t * hmo, void * key);

#define HMO_INIT(hashmap_open, key_type, value_type, hash_function, key_cmp_function) \
_hmo_init((hmo_t *)hashmap_open, sizeof(key_type), _Alignof(key_type), sizeof(value_type), _Alignof(value_type), hash_function, key_cmp_function)


size_t _hmo_max(size_t a, size_t b) {
    return a > b ? a : b;
}

// I did not find this myself
size_t _hmo_offset_of(size_t offset, size_t alignment) {
    return (offset + alignment - 1) & ~(alignment - 1);
}

void _hmo_init(hmo_t *hmo, size_t key_size, size_t key_alignment, size_t value_size, size_t value_alignment, hash_func hash_function, key_cmp_func key_cmp_function) {
    size_t offset = 1; // We know the occupation, which is 1 byte
    size_t key_offset = _hmo_offset_of(offset, key_alignment);
    offset = key_offset + key_size;
    size_t value_offset = _hmo_offset_of(offset, value_alignment);
    offset = value_offset + value_size;
    size_t max_aligmnent = _hmo_max(key_alignment, value_alignment);
    size_t kv_size = _hmo_offset_of(offset, max_aligmnent);

    hmo->size = 0;
    hmo->capacity = 0;
    hmo->key_size = key_size;
    hmo->value_size = value_size;
    hmo->key_offset = key_offset;
    hmo->value_offset = value_offset;
    hmo->kv_size = kv_size;
    hmo->hash_function = hash_function;
    hmo->key_cmp_function = key_cmp_function;
    hmo->data = 0;
}

void hmo_deinit(hmo_t *hmo, allocator_t *a) {
    // TODO: if (but this will not happen in my lifetime) the size of the hashmap is UINT64_MAX, this loop will iterate infinetely :)

    // Free cells
    for(size_t i = 0; i < hmo->capacity; i++) {
        uint8_t * cell = hmo->data + i * hmo->kv_size;
        uint8_t occupied = cell[0];
        if(occupied) {
            void * key = cell + hmo->key_offset;
            void * value = cell + hmo->value_offset;
            if(hmo->value_free_function != NULL) hmo->value_free_function(value, a);
            if(hmo->key_free_function != NULL) hmo->key_free_function(key, a);
        }
        cell[0] = 0;
    }

    allocator_free(a, hmo->data);
    memset(hmo, 0, sizeof(*hmo));
}

HMO_Result _hmo_probing_function(uint8_t * data, void * key, size_t key_offset, size_t kv_size, size_t capacity, hash_func hash_function, key_cmp_func key_cmp_function, uint8_t ** result) {
    uint8_t * buffer = NULL;
    if(result == NULL) result = &buffer;
    uint64_t hash = hash_function(key);

    #ifdef HMO_LINEAR_PROBING

    // Because of the load facter, this should not loop indefinetely
    while(1) {
        size_t index = hash % capacity;
        uint8_t * cell = data + index * kv_size;
        uint8_t occupied = cell[0];
        if(occupied == 0) { *result = cell; return HMO_OK; }
        else {
            uint8_t * old_key = cell + key_offset;
            if(key_cmp_function(old_key, key) == true) {
                // Key already exists, nothing to do
                *result = cell;
                return HMO_KEY_ALREADY_EXISTS_ERROR;
            } else {
                hash += 1;
            }
        }
    }

    #elif HMO_QUADRATIC_PROBING

    #elif HMO_DOUBLE_HASHING

    #else

    #error "Please provide a valid collision resolution strategy!"

    #endif

}

HMO_Result _hmo_expand_to_and_rehash(hmo_t * hmo, size_t new_capacity, allocator_t * a) {
    uint8_t * new_data = allocator_clean_alloc(a, new_capacity, hmo->kv_size);
    //uint8_t * new_data = allocator_aligned_alloc(a, hmo->max_alignment, new_capacity * hmo->kv_size);
    if(!new_data) return HMO_ALLOC_ERROR;

    // Zero out all occupation cells
    //uint8_t * t = new_data;
    //for(size_t i = 0; i < new_capacity; i++) {
    //    new_data[0] = 0;
    //    new_data += hmo->kv_size;
    //}

    if(hmo->data == NULL) {
        hmo->data = new_data;
        hmo->capacity = new_capacity;

        return HMO_OK;
    }

    for(size_t i = 0; i < hmo->capacity; i++) {
        uint8_t * cell = hmo->data + i * hmo->kv_size;
        uint8_t occupied = cell[0];
        if(occupied) {
            void * key = cell + hmo->key_offset;
            uint8_t * place_here = NULL;
            HMO_Result result = _hmo_probing_function(new_data, key, hmo->key_offset, hmo->kv_size, new_capacity, hmo->hash_function, hmo->key_cmp_function, &place_here);
            if(result != HMO_OK) {
                printf("Some error\n");
                return result;
            }
            memcpy(place_here, cell, hmo->kv_size);
        }
    }

    allocator_free(a, hmo->data);
    hmo->data = new_data;
    hmo->capacity = new_capacity;

    return HMO_OK;
}



HMO_Result hmo_insert(hmo_t * hmo, void * key, void * value, allocator_t * a) {
    if(hmo->capacity == 0) {
        HMO_Result result = _hmo_expand_to_and_rehash(hmo, HMO_INIT_CAPACITY, a);
        if(result != HMO_OK) return result;
    }

    if(((hmo->size + 1.0) / hmo->capacity) >= HMO_LOAD_FACTOR) {
        HMO_Result result = _hmo_expand_to_and_rehash(hmo, hmo->capacity * 2, a);
        if(result != HMO_OK) return result;
    }

    uint8_t * place_here;
    HMO_Result result = _hmo_probing_function(hmo->data, key, hmo->key_offset, hmo->kv_size, hmo->capacity, hmo->hash_function, hmo->key_cmp_function, &place_here);

    if(result != HMO_OK) return result;

    // Clone over all data
    place_here[0] = 1;
    memcpy(place_here + hmo->key_offset, key, hmo->key_size);
    memcpy(place_here + hmo->value_offset, value, hmo->value_size);

    hmo->size += 1;

    return HMO_OK;
}

bool hmo_contains_key(hmo_t * hmo, void * key) {
    HMO_Result result = _hmo_probing_function(hmo->data, key, hmo->key_offset, hmo->kv_size, hmo->capacity, hmo->hash_function, hmo->key_cmp_function, NULL);
    return result == HMO_KEY_ALREADY_EXISTS_ERROR;
}

void * hmo_get_value_ref(hmo_t * hmo, void * key) {
    uint8_t * place_here = NULL;
    HMO_Result result = _hmo_probing_function(hmo->data, key, hmo->key_offset, hmo->kv_size, hmo->capacity, hmo->hash_function, hmo->key_cmp_function, &place_here);

    if(result != HMO_KEY_ALREADY_EXISTS_ERROR) return NULL;

    return (void *)(place_here + hmo->value_offset);
}

void hmo_set_key_free_function(hmo_t * hmo, key_free_func key_free_function) {
    hmo->key_free_function = key_free_function;
}

void hmo_set_value_free_function(hmo_t * hmo, value_free_func value_free_function) {
    hmo->value_free_function = value_free_function;
}

#endif
