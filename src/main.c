#include <stdio.h>
#include <kv.h>

int main() {
    kv_t *table = kv_init(3);
    printf("Table pointer: %p\n", table);
    printf("Capacity: %ld\n", table->capacity);

    int return_val = kv_put(table, "first", "first");

    printf("IDX: %d\n", return_val);

    return_val = kv_put(table, "second", "second");
    
    printf("IDX: %d\n", return_val); 

    return_val = kv_put(table, "third", "third");

    printf("IDX: %d\n", return_val);

    char *key = "last";
    char *lookup_value = kv_get(table, key);
    
    if (lookup_value == NULL) {
        printf("Could not find the key %s in table\n", key);
    }
    else {
        printf("%s: %s\n", key, lookup_value);
    }

    printf("Was: %s\n", kv_get(table, "first"));
    kv_delete(table, "first");
    
    printf("Now: %s\n", kv_get(table, "first"));
    
    kv_delete(table, "first");

    for (int i=0; i<table->capacity; i++) {
        if (table->entries[i].key && table->entries[i].key != TOMBSTONE) {
            printf("[%d] %s %s\n", i, table->entries[i].key, table->entries[i].value);
        }
    }

    kv_free(table);

    return 0;
}

