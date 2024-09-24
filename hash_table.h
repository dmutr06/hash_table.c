#ifndef HASH_TABLE_H__
#define HASH_TABLE_H__

#include <stdlib.h>
#include <string.h>

static const float HT_LOAD_FACTOR = 0.6;
static const size_t HT_INITIAL_CAP = 16;

enum _BucketState {
  _BUCKET_EMPTY,
  _BUCKET_OCCUPIED,
  _BUCKET_REMOVED
};

size_t hash(const char *key) {
  size_t hash_value = 0;
  size_t prime = 31;
  
  while (*key)
    hash_value = (hash_value ^ *(key++)) * prime;

  return hash_value;
}

#define REG_HASH_TABLE(type) \
typedef struct _Bucket_##type { \
  enum _BucketState state; \
  char *key; \
  type val; \
} _Bucket_##type; \
\
typedef struct HashTable_##type { \
  size_t cap; \
  size_t size; \
  _Bucket_##type *buckets; \
} HashTable_##type; \
\
void ht_insert_##type (HashTable_##type *this, const char *key, type val); \
type *ht_find_##type (const HashTable_##type *this, const char *key); \
void ht_erase_##type (HashTable_##type *this, const char *key); \
int _internal_ht_insert_##type (HashTable_##type *this, char *key, type val); \
\
void ht_init_##type (HashTable_##type *table) { \
  table->cap = HT_INITIAL_CAP; \
  table->size = 0; \
  table->buckets = malloc(table->cap * sizeof(_Bucket_##type)); \
  for (size_t i = 0; i < table->cap; ++i) { \
      table->buckets[i].state = _BUCKET_EMPTY; /* Initialize bucket states to _BUCKET_EMPTY */ \
  } \
} \
void _ht_resize_##type (HashTable_##type *this) { \
  const size_t old_cap = this->cap; \
  this->cap *= 2; \
  _Bucket_##type *old_buckets = this->buckets; \
  this->buckets = malloc(this->cap * sizeof(_Bucket_##type)); \
  for (size_t i = 0; i < this->cap; ++i) { \
      this->buckets[i].state = _BUCKET_EMPTY; /* Initialize bucket states to _BUCKET_EMPTY */ \
  } \
  for (size_t i = 0; i < old_cap; ++i) { \
    _Bucket_##type *bucket = old_buckets + i; \
    switch (bucket->state) { \
      case _BUCKET_EMPTY: \
        break; \
      case _BUCKET_OCCUPIED: \
        _internal_ht_insert_##type (this, bucket->key, bucket->val); \
        break; \
      case _BUCKET_REMOVED: \
        free(bucket->key); \
        break; \
    } \
  } \
  free(old_buckets); \
} \
\
type *ht_find_##type (const HashTable_##type *this, const char *key) { \
  const size_t hashed_key = hash(key); \
  for (size_t i = 0; i < this->cap; ++i) { \
    _Bucket_##type *bucket = this->buckets + ((hashed_key + i) % this->cap); \
    switch (bucket->state) { \
      case _BUCKET_EMPTY: \
        break; \
      case _BUCKET_OCCUPIED: \
        if (strcmp(key, bucket->key) == 0) return &bucket->val; \
        break; \
      case _BUCKET_REMOVED: \
        if (strcmp(key, bucket->key) == 0) return NULL; \
        break; \
    } \
  } \
  return NULL; \
} \
\
int _internal_ht_insert_##type (HashTable_##type *this, char *key, type val) { \
  const size_t hashed_key = hash(key); \
  for (size_t i = 0; i < this->cap; ++i) { \
    _Bucket_##type *bucket = this->buckets + ((hashed_key + i) % this->cap); \
    switch (bucket->state) { \
      case _BUCKET_EMPTY: \
        bucket->key = key; \
        bucket->val = val; \
        bucket->state = _BUCKET_OCCUPIED; \
        return 1; \
      case _BUCKET_OCCUPIED: \
        if (strcmp(key, bucket->key) == 0) { \
          bucket->val = val; \
          return 0; \
        } \
        break; \
      case _BUCKET_REMOVED: \
        free(bucket->key); \
        bucket->key = key; \
        bucket->val = val; \
        bucket->state = _BUCKET_OCCUPIED; \
        return 1; \
    } \
  } \
} \
void ht_insert_##type (HashTable_##type *this, const char *key, type val) { \
  if (this->cap * HT_LOAD_FACTOR <= this->size) _ht_resize_##type (this); \
  char *new_key = malloc(strlen(key) + 1); \
  strcpy(new_key, key); \
  this->size += _internal_ht_insert_##type (this, new_key, val); \
}\
\
void ht_erase_##type (HashTable_##type *this, const char *key) { \
  const size_t hashed_key = hash(key); \
  for (size_t i = 0; i < this->cap; ++i) { \
    _Bucket_##type *bucket = this->buckets + ((hashed_key + i) % this->cap); \
    switch (bucket->state) { \
      case _BUCKET_EMPTY: \
        return; \
      case _BUCKET_OCCUPIED: \
        if (strcmp(key, bucket->key) != 0) break; \
        --this->size; \
        bucket->state = _BUCKET_REMOVED; \
        break; \
      case _BUCKET_REMOVED: \
        if (strcmp(key, bucket->key) == 0) return; \
        break; \
    } \
  } \
} \
\
void ht_deinit_##type (HashTable_##type *this) { \
    for (size_t i = 0; i < this->cap; i++) { \
        _Bucket_##type *bucket = this->buckets + i; \
        switch (bucket->state) { \
            case _BUCKET_EMPTY: \
                continue; \
            case _BUCKET_OCCUPIED: \
                free(bucket->key); \
                continue; \
            case _BUCKET_REMOVED: \
                continue; \
        } \
    } \
    free(this->buckets); \
} \

#endif
