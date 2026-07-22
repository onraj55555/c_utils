#include <stdlib.h>
#define ALLOCATOR_HEAP_ALLOCATOR
#include "hashmap_open.h"
#include "allocator.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

uint64_t int_hash_function(void * key) {
    return (uint64_t) *(int *)key;
}

bool int_cmp_function(void * key1, void * key2) {

    return *(int *)key1 == *(int *)key2;
}

void test_int_int(allocator_t * a) {
    printf("===== TESTING hmo<int,int> =====\n");
    hmo_t hmii = { 0 };
    HMO_INIT(&hmii, int, int, &int_hash_function, &int_cmp_function);

    printf("> Inserting keys\n");
    for(int i = 999; i >= 0; i--) {
        int value = 420;
        HMO_Result result = hmo_insert(&hmii, &i, &value, a);
        if(result != HMO_OK) {
            printf("! Failed to insert key %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    printf("> Checking keys\n");
    for(int i = 0; i < 1000; i++) {
        bool result = hmo_contains_key(&hmii, &i);
        if(result == false) {
            printf("! Key %d not found\n", i);
            exit(EXIT_FAILURE);
        }
    }

    printf("> Testing non existing key\n");
    int key_not_exists = -1;
    if(hmo_contains_key(&hmii, &key_not_exists) != false) {
        printf("! Found key -1! THIS DOES NOT EXIST\n");
        exit(EXIT_FAILURE);
    }

    printf("===== TESTING hmo<int,int> DONE =====\n");
}

uint64_t string_hash_function(void * key) {
    char * string = (char *)key;
    uint64_t hash = 0;
    while(*string) {
        hash = hash * 10 + *string;
        string++;
    }

    return hash;
}

bool string_cmp_function(void * key1, void * key2) {
    return strcmp((char *)key1, (char *)key2) == 0;
}

void string_free_function(void * key, allocator_t * a) {
    char * string = key;
    printf("Freeing %s\n", string);
}

void int_free_function(void * value, allocator_t * a) {
    int * i = value;
    printf("Freeing %d\n", *i);
}

void test_string_int(allocator_t * a) {
    printf("===== TESTING hmo<char *, int> =====\n");

    hmo_t hmsi = { 0 };
    HMO_INIT(&hmsi, char *, int, &string_hash_function, &string_cmp_function);
    hmo_set_key_free_function(&hmsi, &string_free_function);
    hmo_set_value_free_function(&hmsi, int_free_function);

    printf("> Inserting keys\n");
    char * names[] = {"Bob", "Alice", "Charley"};
    int ages[] = {69, 420, 80085};

    for(int i = 0; i < sizeof(names) / sizeof(names[0]); i++) {
        assert(hmo_insert(&hmsi, names[i], &ages[i], a) == HMO_OK || "Error while inserting");
    }

    printf("> Querying values from keys\n");
    for(int i = 0; i < sizeof(names) / sizeof(names[0]); i++) {
        char * name = names[i];
        int * age_queried = (int *)hmo_get_value_ref(&hmsi, name);
        int age_real = ages[i];
        printf("%s age (queried): %d, %s age (real): %d\n", name, *age_queried, name, age_real);
        if(*age_queried != age_real) {
            printf("Queried age for %s is not equal to real age, expected %d, got %d\n", name, age_real, *age_queried);
        }
    }

    printf("> Deinit\n");
    hmo_deinit(&hmsi, a);

    printf("===== TESTING hmo<char *, int> DONE =====");
}

typedef struct {
    char a[1000];
} i_am_big;

void test_big_stuct(allocator_t * a) {
    printf("===== TESTING hmo<int, i_am_big> =====\n");

    hmo_t hmo = { 0 };
    HMO_INIT(&hmo, int, i_am_big, &int_hash_function, &int_cmp_function);

    i_am_big iab = { 0 };

    printf("> Inserting lots of big structs\n");
    for(int i = 0; i < 1000000; i++) {
        HMO_Result result = hmo_insert(&hmo, &i, &iab, a);
        if(result != HMO_OK) {
            printf("Failed to insert %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    printf("===== TESTING hmo<int, i_am_big> DONE =====\n");
}

int main() {
    allocator_t a;
    allocator_new_heap_allocator(&a);

    test_int_int(&a);
    test_string_int(&a);
    test_big_stuct(&a);

    return 0;
}
