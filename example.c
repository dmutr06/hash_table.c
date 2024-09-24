#include <stdio.h>

#include "hash_table.h"

REG_HASH_TABLE(int);

int main() {
    HashTable_int table;

    ht_init_int(&table);
    ht_insert_int(&table, "a", 10);
    ht_insert_int(&table, "b", 11);
    ht_insert_int(&table, "c", 12);

    printf("%d\n", *ht_find_int(&table, "a"));
    printf("%d\n", *ht_find_int(&table, "b"));
    printf("%d\n", *ht_find_int(&table, "c"));

    ht_deinit_int(&table);

    return 0;
}
