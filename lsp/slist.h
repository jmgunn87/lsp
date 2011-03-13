#ifndef SLIST_H
#define SLIST_H

#pragma once 
#include "stdafx.h"

/**
 * these are the links an slist will
 * store.
 **/
typedef struct _slist_elem
{
  void* _data;
  struct _slist_elem* _next;
} slist_elem;

slist_elem* new_slist_elem(void* data);
void slist_elem_destroy(slist_elem* sle,
                        void(*destructor)(void*));
/**
 * slist_elem container, slist
 **/
typedef struct _slist
{
  size_t _size;
  slist_elem* _head;
  slist_elem* _tail;
} slist;

slist* new_slist();
void slist_destroy(slist* list,
                   void(*destructor)(void*));

/********************
 * stack operations *
 ********************/
void slist_pushb(slist* list,
                 void* data);
void slist_pushf(slist* list,
                 void* data);
void slist_push_at(slist* list,
                   void* data,
                   int n);

void* slist_popb(slist* list);
void* slist_popf(slist* list);
void* slist_popbn(slist* list,
                  int n);
void* slist_popfn(slist* list,
                  int n);

#define slist_peekb(list)( !list||list->_size<1?0:list->_tail->_data )
#define slist_peekf(list)( !list||list->_size<1?0:list->_head->_data )
#define slist_peekbn(list,n)( slist_at(list,list->_size-n) )
#define slist_peekfn(list,n)( slist_at(list,n) )


void* slist_at(slist* list,
               int index);
int slist_indexof(slist* list,
                  void* comp_dat,
                  char(*comp_fn)(void*,void*));
void slist_reverse(slist* list);
void* slist_remove(slist* list,
                   int index);
slist* slist_subseq(slist* list,
                    int from,
                    int to);
slist* slist_copy(slist* list,
                  void* user_data,
                  void*(*copier)(void*,void*));
slist* slist_rcopy(slist* list,
                   void* user_data,
                   void*(*copier)(void*,void*));
void slist_each(slist* list,
                void* user_data,
                void(*function)(void*,void*));
void slist_each_parallel(slist* lists,
                         void* user_data,
                         void(*function)(void*,void*));



//slist* slist_swap(slist* list,
//                  int a,
//                  int b);
//void slist_insert(slist* list,
//                  void* data,
//                  int after);
//void* slist_remove(slist* list,
//                   int index);
//void* slist_delete(slist* list,
//                   int index);




//void slist_append(slist* list,
//                  void* data);
//void slist_insert(slist* list,
//                  int index,
//                  void* data);
//void* slist_at(slist* list,
//               int index);
//
//slist_elem* slist_elem_at(slist* list,
//                          int index);
//
//slist* slist_range(slist* list,
//                   int from,
//                   int to);
//
//unsigned char slist_remove_at(slist* list,
//                              int index);
//
//void* slist_pop(slist* list);
//void* slist_peek(slist* list);
//void* slist_peekn(slist* list,
//                  int n);
//void slist_push(slist* list,
//                void* data);
//
//void slist_remove_tail(slist* list);
//
//void* slist_find(slist* list,
//                 int* index,
//                 void* comp_dat,
//                 char(*comp_fn)(void*,void*));
//
//slist* slist_split(slist* list,
//                   int index);
//
//slist_elem* slist_tail(slist* list);
//
//void slist_clear(slist* list,
//                 void(*dealloc)(void*));
//

#endif