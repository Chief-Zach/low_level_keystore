#include <stdio.h>
#include <kv.h>

int main() {
    kv_t *table = kv_init(3);
    printf("Table pointer: %p\n", table);
    printf("Capacity: %ld\n", table->capacity);
    kv_put(table, "hehe", "haha");

    for (int i=0; i<table->capacity; i++) {
        if (table->entries[i].key) {
            printf("%s %s\n", table->entries[i].key, table->entries[i].value);
        }
    }

    kv_free(table);

    return 0;
}

