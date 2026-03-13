#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <kv.h>

static int tests_run = 0;
static int tests_passed = 0;

static void check(const char *test_name, int passed) {
    tests_run++;
    if (passed) {
        tests_passed++;
        printf("PASS: %s\n", test_name);
    } else {
        printf("FAIL: %s\n", test_name);
    }
}

int main() {
    kv_t *db;
    int ret;
    char *val;

    // --- kv_init ---
    db = kv_init(0);
    check("init with capacity 0 returns NULL", db == NULL);

    db = kv_init(8);
    check("init with valid capacity returns non-NULL", db != NULL);
    check("init sets capacity correctly", db->capacity == 8);
    check("init sets count to 0", db->count == 0);

    // --- kv_put basic ---
    ret = kv_put(db, "foo", "bar");
    check("put returns valid index", ret >= 0);
    check("put increments count", db->count == 1);

    ret = kv_put(db, "hello", "world");
    check("put second key returns valid index", ret >= 0);
    check("count is 2 after two puts", db->count == 2);
    printf("Count: %ld\n", db->count);

    // --- kv_get basic ---
    val = kv_get(db, "foo");
    check("get returns correct value for foo", val != NULL && strcmp(val, "bar") == 0);

    val = kv_get(db, "hello");
    check("get returns correct value for hello", val != NULL && strcmp(val, "world") == 0);

    val = kv_get(db, "nonexistent");
    check("get returns NULL for missing key", val == NULL);

    // --- kv_put update ---
    ret = kv_put(db, "foo", "updated");
    check("put on existing key returns valid index", ret >= 0);
    check("count unchanged after update", db->count == 2);

    val = kv_get(db, "foo");
    check("get returns updated value", val != NULL && strcmp(val, "updated") == 0);

    // --- kv_delete basic ---
    ret = kv_delete(db, "foo");
    check("delete returns valid index", ret >= 0);
    check("delete decrements count", db->count == 1);

    val = kv_get(db, "foo");
    check("get returns NULL after delete", val == NULL);

    // --- kv_delete nonexistent ---
    ret = kv_delete(db, "foo");
    check("delete of already deleted key returns -1", ret == -1);

    ret = kv_delete(db, "nonexistent");
    check("delete of never-inserted key returns -1", ret == -1);

    // --- tombstone reuse ---
    ret = kv_put(db, "foo", "reborn");
    check("put into tombstone slot succeeds", ret >= 0);
    check("count increments after tombstone reuse", db->count == 2);

    val = kv_get(db, "foo");
    check("get returns correct value after tombstone reuse", val != NULL && strcmp(val, "reborn") == 0);

    // --- NULL argument handling ---
    check("put with NULL db returns -1", kv_put(NULL, "k", "v") == -1);
    check("put with NULL key returns -1", kv_put(db, NULL, "v") == -1);
    check("put with NULL value returns -1", kv_put(db, "k", NULL) == -1);
    check("get with NULL db returns NULL", kv_get(NULL, "k") == NULL);
    check("get with NULL key returns NULL", kv_get(db, NULL) == NULL);
    check("delete with NULL db returns -1", kv_delete(NULL, "k") == -1);
    check("delete with NULL key returns -1", kv_delete(db, NULL) == -1);

    // --- fill table ---
    kv_free(db);
    db = kv_init(4);

    kv_put(db, "a", "1");
    kv_put(db, "b", "2");
    kv_put(db, "c", "3");
    kv_put(db, "d", "4");
    check("count is 4 after filling table", db->count == 4);

    val = kv_get(db, "a");
    check("get a from full table", val != NULL && strcmp(val, "1") == 0);
    val = kv_get(db, "d");
    check("get d from full table", val != NULL && strcmp(val, "4") == 0);

    // --- delete and re-insert in full table ---
    kv_delete(db, "b");
    check("count is 3 after delete from full table", db->count == 3);

    ret = kv_put(db, "e", "5");
    check("put into previously full table after delete succeeds", ret >= 0);
    check("count is 4 after re-insert", db->count == 4);

    val = kv_get(db, "e");
    check("get new key after tombstone reuse in full table", val != NULL && strcmp(val, "5") == 0);

    val = kv_get(db, "b");
    check("deleted key still not found after reuse", val == NULL);

    kv_free(db);

    printf("\n%d/%d tests passed\n", tests_passed, tests_run);
    return tests_passed == tests_run ? 0 : 1;
}
