#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL
#define INIT_CAPACITY 10
#define LOAD_FACTOR 0.75
#define SCALE_FACTOR 1.5

typedef struct hash_table_entry hash_table_entry;

struct hash_table_entry
{
    char* key;
    char* val;
    hash_table_entry* next;
};

typedef struct 
{
    hash_table_entry** buckets;
    int capacity;
    int size;
} hash_table;

static uint64_t hash_key(const char* key);

int ht_add_value(hash_table* ht, const char* key, const char* val);

char* ht_get_value(hash_table* ht, const char* key);

void ht_destroy(hash_table* ht);