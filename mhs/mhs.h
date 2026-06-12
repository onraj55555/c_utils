// Note: open addressing

#ifndef MHS_H_UTILS
#define MHS_H_UTILS

#define MHS_CONCAT2(a, b) a##b
#define MHS_CONCAT(a, b) MHS_CONCAT2(a, b)

#define MHS_OK 0
#define MHS_ALLOC_ERROR 1
#define MHS_RESIZE_ERROR 2
#define MHS_ADD_ERROR 3

#endif

#define MHS_TYPE int
#ifdef MHS_TYPE

#include <stdint.h>
#include "allocator.h"
#include <string.h>

#ifndef MHS_PREFIX
#define MHS_PREFIX MHS_CONCAT(hs_, MHS_TYPE)
#endif

#ifndef MHS_START_CAPACITY
#define MHS_START_CAPACITY 64
#endif

#ifndef MHS_LOAD_FACTOR
#define MHS_LOAD_FACTOR 0.6
#endif

#define MHS_KEY MHS_CONCAT(MHS_PREFIX, _key)

extern int MHS_CONCAT(MHS_PREFIX, _error);

typedef struct {
    int occupied;
    int64_t hash; // To not recompute the hash, these
    MHS_TYPE key;
} MHS_KEY;

typedef struct {
    int (* cmp_function)(MHS_TYPE * key1, MHS_TYPE * key2);
    int64_t (* hash_function)(MHS_TYPE * key);
    int64_t count;
    int64_t capacity;
    MHS_KEY * buffer;
} MHS_PREFIX;

// void hs_int_init(hs_int * self, int64_t (* hash_function)(void * key), int (* cmp_function)(void * key1, void * key2), allocator_t * a)
void MHS_CONCAT(MHS_PREFIX, _init)(MHS_PREFIX * self, int64_t (* hash_function)(MHS_TYPE * key), int (* cmp_function)(MHS_TYPE * key1, MHS_TYPE * key2), allocator_t * a);

// void hs_int_deinit(hs_int * self, allocator_t * a)
void MHS_CONCAT(MHS_PREFIX, _deinit)(MHS_PREFIX * self, allocator_t * a);

// void hs_int_init_reserve(hs_int * self, int64_t capacity, int64_t (* hash_function)(void * key), int (* cmp_function)(void * key1, void * key2), allocator_t * a)
void MHS_CONCAT(MHS_PREFIX, _init_reserve)(MHS_PREFIX * self, int64_t capacity, int64_t (* hash_function)(MHS_TYPE * key), int (* cmp_function)(MHS_TYPE * key1, MHS_TYPE * key2), allocator_t * a);

// void hs_int_add(hs_int * self, int * key, allocator_t * a)
void MHS_CONCAT(MHS_PREFIX, _add)(MHS_PREFIX * self, MHS_TYPE * key, allocator_t * a);

// void hs_int_remove(hs_int * int, int * key);
void MHS_CONCAT(MHS_PREFIX, _remove)(MHS_PREFIX * self, MHS_TYPE * key);

// void hs_int_contains(hs_int * self, int * key)
int MHS_CONCAT(MHS_PREFIX, _contains)(MHS_PREFIX * self, MHS_TYPE * key);

static inline void MHS_CONCAT(MHS_PREFIX, _init)(MHS_PREFIX * self, int64_t (* hash_function)(MHS_TYPE * key), int (* cmp_function)(MHS_TYPE * key1, MHS_TYPE * key2), allocator_t * a) {
    MHS_CONCAT(MHS_PREFIX, _init_reserve)(self, MHS_START_CAPACITY, hash_function, cmp_function, a);
}

static inline void MHS_CONCAT(MHS_PREFIX, _deinit)(MHS_PREFIX * self, allocator_t * a) {
    allocator_free(a, self->buffer);
    self->count = 0;
    self->capacity = 0;
    self->cmp_function = 0;
    self->hash_function = 0;
    MHS_CONCAT(MHS_PREFIX, _error) = MHS_OK;
}

static inline void MHS_CONCAT(MHS_PREFIX, _init_reserve)(MHS_PREFIX * self, int64_t capacity, int64_t (* hash_function)(MHS_KEY * key), int (* cmp_function)(MHS_KEY * key1, MHS_KEY * key2), allocator_t * a) {
    self->buffer = (MHS_KEY *)allocator_clean_alloc(a, capacity, sizeof(MHS_KEY));
    if(!self->buffer) {
        MHS_CONCAT(MHS_PREFIX, _error) = MHS_ALLOC_ERROR;
        return;
    }
    self->capacity = capacity;
    self->count = 0;
    self->hash_function = hash_function;
    self->cmp_function = cmp_function;
}

static inline void MHS_CONCAT(MHS_TYPE, _expand)(MHS_PREFIX * self, allocator_t * a) {
    int64_t new_capacity = self->capacity * 2;
    MHS_KEY * new_buffer = (MHS_KEY *)allocator_clean_alloc(a, new_capacity, sizeof(MHS_KEY));

    if(!new_buffer) {
        MHS_CONCAT(MHS_PREFIX, _error) = MHS_ALLOC_ERROR;
        return;
    }

    int64_t count = 0;
    for(int64_t i = 0; i < self->capacity; i++) {
        MHS_KEY * key = &self->buffer[i];
        if(!key->occupied) continue;

        count += 1;
        int64_t new_location = key->hash % new_capacity;
        int64_t new_location2 = new_location;
        int added = 0;

        // First time legit use of do-while
        do {
            if(!new_buffer[new_location].occupied) {
                new_buffer[new_location2].occupied = 1;
                new_buffer[new_location2].hash = key->hash;
                new_buffer[new_location2].key = key->key;
                added = 1;
                break;
            }
            new_location2 += 1;
            new_location2 %= new_capacity;
        } while (new_location2 != new_location);

        // This should never happen because load will always be < 1, so there is always a spot
        if(!added) {
            MHS_CONCAT(MHS_PREFIX, _error) = MHS_RESIZE_ERROR;
            allocator_free(a, new_buffer);
            return;
        }

        // Early escape from loop to not iterate the left over empty slots (if any)
        if(count == self->count) {
            break;
        }
    }

    void * temp = self->buffer;
    self->buffer = new_buffer;
    self->capacity = new_capacity;
    allocator_free(a, temp);

    MHS_CONCAT(MHS_PREFIX, _error) = MHS_OK;
}

// void hs_int_add(hs_int * self, int * key, allocator_t * a)
static inline void MHS_CONCAT(MHS_PREFIX, _add)(MHS_PREFIX * self, MHS_TYPE * key, allocator_t * a) {
    if(self->count / self->capacity > MHS_LOAD_FACTOR) {
        MHS_CONCAT(MHS_TYPE, _expand)(self, a);
        if(MHS_CONCAT(MHS_PREFIX, _error) != MHS_OK) return;
    }

    int64_t hash = self->hash_function(key);
    int64_t location = hash % self->capacity;
    int64_t location2 = location;
    int added = 0;
    do {
        if(!self->buffer[location2].occupied) {
            self->buffer[location2].occupied = 1;
            self->buffer[location2].hash = hash;
            self->buffer[location2].key = *key;
            added = 1;
            break;
        } else {
            MHS_TYPE * other_key = &self->buffer[location2].key;
            int64_t other_hash = self->buffer[location2].hash;

            // Possible that the key is already in there
            if(hash == other_hash && self->cmp_function(key, other_key)) {
                // Key is already in there, just return
                MHS_CONCAT(MHS_PREFIX, _error) = MHS_OK;
                return;
            }
        }

        location2 += 1;
        location2 %= self->capacity;
    } while(location2 != location);

    if(!added) {
        MHS_CONCAT(MHS_PREFIX, _error) = MHS_ADD_ERROR;
        return;
    }

    MHS_CONCAT(MHS_PREFIX, _error) = MHS_OK;
    return;
}

// void hs_int_remove(hs_int * int, int * key);
void MHS_CONCAT(MHS_PREFIX, _remove)(MHS_PREFIX * self, MHS_TYPE * key) {
    int64_t hash = self->hash_function(key)
}

#undef MHS_KEY
#undef MHS_PREFIX
#undef MHS_TYPE

#endif