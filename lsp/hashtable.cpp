#pragma once
#include "stdafx.h"
#include "hashtable.h"

/**
 * internally used structure
 * to store entries
 **/
typedef struct _ht_entry
{
  const char* name;
  void* data;
} ht_entry;

ht_entry* new_ht_entry(const char* name,
                       void* data)
{
  ht_entry* he=(ht_entry*)malloc(sizeof(ht_entry));
  he->name=name;
  he->data=data;
  return he;
}
void destroy_ht_entry(ht_entry* he,
                      void(*destructor)(void*))
{
  /* require a valid destructor */
  if(!he||!destructor)
    return;
  he->name=0;
  destructor(he->data);
  he->data=0;
  free(he);
}

hash_table* new_hash_table(int buckets,
                           unsigned int(*hash_function)(const void*,unsigned int))
{
  int i=0;
  hash_table* ht=(hash_table*)malloc(sizeof(hash_table));
  ht->num_buckets=buckets;/* store the table size */
  ht->hash_fn=hash_function;/* store the hash function pointer */
  ht->buckets=(slist**)malloc(sizeof(slist*)*buckets);/* allocate the buckets array(the table) */
  for(;i<buckets;++i)/* allocate each bucket */
    ht->buckets[i]=new_slist();
  return ht;
}

void hash_table_destroy(hash_table* ht,
                        void(*destructor)(void*))
{
  int b=0;           /* bucket iterator */
  slist_elem* sle=0; /* bucket list iterator */

  /* require a valid destructor */
  if(!ht||!destructor)
    return;

  /* cycle through each bucket */
  for(;b<ht->num_buckets;++b)
  {
    /* cycle through the current buckets list 
       and destroy each ht entry while passing on
       @destructor to destroy the data associated
       with the entry */
    sle=ht->buckets[b]->_head;
    while(sle)
    {
      destroy_ht_entry((ht_entry*)sle->_data,destructor);
      sle=sle->_next;
    }
    /* finally destroy the buckets list itself */
    slist_destroy(ht->buckets[b],free);
  }
}

char hash_table_put(hash_table* ht,
                    const char* name,
                    const void* data)
{
  if(!ht||!ht->hash_fn||!ht->buckets||!name)
    return 0;
  if(!hash_table_get(ht,name))
  {
    slist_pushf(ht->buckets[ht->hash_fn(name,ht->num_buckets)],
                (void*)new_ht_entry(name,(void*)data));
    return 1;
  }return 0;
}

void* hash_table_get(hash_table* ht,
                     const char* name)
{
  slist_elem* sle=0;
  unsigned int hash=0;

  if(!ht||!ht->hash_fn||!ht->buckets||!name)
    return 0;

  hash=ht->hash_fn(name,ht->num_buckets);
  sle=ht->buckets[hash]->_head;
  while(sle)
  {
    if(strcmp(((ht_entry*)sle->_data)->name,name)==0)
      return ((ht_entry*)sle->_data)->data;
    sle=sle->_next;
  }
  return 0;
}
void* hash_table_remove(hash_table* ht,
                        const char* name)
{
  slist_elem* sle=0;
  int index=1;
  unsigned int hash=0;

  if(!ht||!ht->hash_fn||!ht->buckets||!name)
    return 0;

  hash=ht->hash_fn(name,ht->num_buckets);
  sle=ht->buckets[hash]->_head;
  while(sle)
  {
    if(strcmp(((ht_entry*)sle->_data)->name,name)==0)
      return slist_remove(ht->buckets[hash],index);
    ++index;
    sle=sle->_next;
  }
  return 0;
}