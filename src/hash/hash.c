#include <stdlib.h>
#include <strings.h>

#include "hash.h"

// Return 64-bit FNV-1a hash for key (NUL-terminated). See description:
// https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
static uint64_t hash_key(const char* key) 
{
    uint64_t hash = FNV_OFFSET;
    for (const char* p = key; *p; p++) {
        hash ^= (uint64_t)(unsigned char)(*p);
        hash *= FNV_PRIME;
    }
    return hash;
}

void resize(hash_table* ht)
{
    int old_capa = ht->capacity;
    int new_capa = ht->capacity * SCALE_FACTOR;

    hash_table_entry** new_buckets = calloc(new_capa, sizeof(hash_table_entry*));
    hash_table_entry** old_buckets = ht->buckets;

    ht->buckets = new_buckets;
    ht->capacity = new_capa;
    ht->size = 0;

    for (int i = 0; i < old_capa; i++)
    {
        hash_table_entry* cur = old_buckets[i];
        while (cur != NULL)
        {
            add_value(ht, cur->key, cur->val);
            hash_table_entry* next = cur->next;
            free(cur->key);
            free(cur->val);
            free(cur);

            cur = next;
        }
    }

    if (old_buckets != NULL)
    {
        free(old_buckets);
    }
}

void ht_destroy(hash_table* ht)
{
    for (int i = 0; i < ht->capacity; i++)
    {
        hash_table_entry* cur = ht->buckets[i];
        while (cur != NULL)
        {
            hash_table_entry* next = cur->next;
            free(cur->key);
            free(cur->val);
            free(cur);

            cur = next;
        }
    }

    free(ht->buckets);
}

void ensure_capa(hash_table* ht)
{
    if (ht->size >= ht->capacity * LOAD_FACTOR)
    {
        resize(ht);
    }
}

int ht_add_value(hash_table* ht, const char* key, const char* val)
{
    if (ht->buckets == NULL) {
        ht->buckets = malloc(INIT_CAPACITY * sizeof(hash_table_entry*));
        ht->capacity = INIT_CAPACITY;
        ht->size = 0;
    }

    ensure_capa(ht);

    uint64_t hash = hash_key(key);
    uint32_t idx = hash % ht->capacity;

    hash_table_entry* new_he = malloc(sizeof(hash_table_entry));
    new_he->key = strdup(key);
    new_he->val = strdup(val);
    new_he->next = NULL;
    ht->size++;

    hash_table_entry* he = ht->buckets[idx];
    if (he == NULL) {
        ht->buckets[idx] = new_he;
    } else {
        hash_table_entry* cur = he;
        while (cur->next != NULL) {
            if (strcmp(cur->key, key) == 0) {
                free(cur->val);
                cur->val = strdup(val);
                free(new_he->key);
                free(new_he->val);
                free(new_he);
                ht->size--;
                return 0;
            }
            cur = cur->next;
        }
        if (strcmp(cur->key, key) == 0) {
            free(cur->val);
            cur->val = strdup(val);
            free(new_he->key);
            free(new_he->val);
            free(new_he);
            ht->size--;
        } else {
            cur->next = new_he;
        }
    }
    
    return 0;
}

char* ht_get_value(hash_table* ht, const char* key)
{
    uint64_t hash = hash_key(key);
    uint32_t idx = hash % ht->capacity;

    hash_table_entry* he = ht->buckets[idx];

    hash_table_entry* cur = he;
    while(cur != NULL)
    {
        if (strcmp(cur->key, key) == 0) {
            return cur-> val;
        }
        cur = cur->next;
    }

    return NULL;
}