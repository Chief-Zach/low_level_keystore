#include <kv.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


size_t hash(const char *value, size_t capacity) {
    size_t hash = 0x13371337deadbeef;

    while (*value) {
        hash ^= *value;
        hash = hash << 8;
        hash += *value;
    
        value++;
    }

    return hash % capacity;

}


int kv_delete(kv_t *db, const char *key) {
    if (!db || !key) {
        return -1;
    }

    size_t idx = hash(key, db->capacity);

    for (int i=0; i<db->capacity - 1; i++) {
        
        printf("%d\n", i);
        size_t real_idx = (idx + i) % db->capacity;

        kv_entry_t *entry = &db->entries[real_idx];
        
        if (entry->key == NULL) return -1;

        if (entry->key && entry->key != TOMBSTONE && !strcmp(entry->key, key)) {
            free(entry->key);
            free(entry->value);
            db->count--;
            entry->key = TOMBSTONE;
            entry->value = NULL;
            return real_idx;
        }
    }
    return -1;
}

char *kv_get(kv_t *db, const char *key) {
    if (!db || !key) {
        return NULL;
    }

    size_t idx = hash(key, db->capacity);

    for (int i=0; i<db->capacity - 1; i++) {
        size_t real_idx = (idx + i) % db->capacity;
        
        kv_entry_t *entry = &db->entries[real_idx];

        if (!entry->key) return NULL;
        if (entry->key == TOMBSTONE) continue;

        if (entry->key && entry->key != TOMBSTONE && !strcmp(entry->key, key)) {
            return entry->value;
        }
    }

    return NULL;
}

int kv_put(kv_t *db, const char *key, const char *value) {
    if (!db || !key || !value) {
        return -1;
    }

    size_t idx = hash(key, db->capacity);

    for (int i=0; i<db->capacity; i++) {
        size_t real_idx = (idx + i) % db->capacity;

        kv_entry_t *entry = &db->entries[real_idx];

        if (entry->key && entry->key != TOMBSTONE && !strcmp(entry->key, key)) {
            char *newval = strdup(value);
            if (newval == NULL) {
                return -1;
            }

            entry->value = newval;
            return real_idx;
        }

        else if (!entry->key || entry->key == TOMBSTONE) {
            char *newval = strdup(value);
            char *newkey = strdup(key);

            if (!newval || !newkey) {
                free(newval);
                free(newkey);
                return -1; 
            }
            
            entry->value = newval;
            entry->key = newkey;
            db->count++;
            return real_idx;
        }
    }

    return -2;
}

kv_t *kv_init(size_t capacity) {
    if (capacity == 0) {
        return NULL;
    }

    kv_t *table = malloc(sizeof(kv_t));
    if (table == NULL) {
        return NULL;
    }

    table->capacity = capacity;
    table->count = 0;

    table->entries = calloc(sizeof(kv_entry_t), capacity);

    if (table->entries == NULL) {
        return NULL;
    }

    return table;
}

void kv_free(kv_t *db) {
    free(db);
}
