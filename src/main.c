#include <stdio.h>
#include <kv.h>

int main() {
    kv_t *table = kv_init(3);
    printf("Table pointer: %p\n", table);
    printf("Capacity: %ld\n", table->capacity);
    kv_free(table);

    return 0;
}

