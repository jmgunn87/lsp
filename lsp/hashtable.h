#ifndef HASHTABLE_H
#define HASHTABLE_H

#pragma once
#include "slist.h"

typedef struct _hash_table
{
  int num_buckets;
  unsigned int(*hash_fn)(const void*,unsigned int);
  slist** buckets;
} hash_table;

hash_table* new_hash_table(int buckets,
                           unsigned int(*hash_function)
                           (const void*,unsigned int));
void hash_table_destroy(hash_table* ht,
                        void(*destructor)(void*));
char hash_table_put(hash_table* ht,
                    const char* name,
                    const void* data);
void* hash_table_get(hash_table* ht,
                     const char* name);
void* hash_table_remove(hash_table* ht,
                        const char* name);
#endif