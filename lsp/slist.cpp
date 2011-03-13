#pragma once 
#include "stdafx.h"
#include "slist.h"
#include <stdlib.h>
#include <string.h>

/************************************
 *>> private forward declarations <<*
 ************************************/
static slist_elem* slist_elem_at(slist* list,
                                 int index);

/**************************
 *>> public definitions <<*
 **************************/

/******************************
 * constructor for slist_elem *
 ******************************/
slist_elem* new_slist_elem(void* data)
{
  slist_elem* sle=(slist_elem*)malloc(sizeof(slist_elem));
  sle->_data=data;
  sle->_next=0;
  return sle;
}
/*****************************
 * destructor for slist_elem *
 *****************************/
void slist_elem_destroy(slist_elem* sle,
                        void(*destructor)(void*))
{
  /* require a valid destructor to be passed */
  if(!sle||!destructor)
    return;

  /* call the destructor */
  destructor(sle->_data);
  /* free the element and zero its pointer */
  free(sle);
  sle=0;
}

/******************************************
 * destructors and constructors for slist *
 ******************************************/

slist* new_slist()
{
  slist* nsl=(slist*)malloc(sizeof(slist));
  nsl->_head=0;
  nsl->_tail=0;
  nsl->_size=-1;
  return nsl;
}

void slist_destroy(slist* list,
                   void(*destructor)(void*))
{
  slist_elem* curr=0; /* current element cursor */
  slist_elem* next=0; /* lookahead element */

  if(!list||!destructor)
    return;

  /* iterate the list */
  curr=list->_head;
  while(curr&&curr->_next)
  {
    next=curr->_next;/* save the lookahead */
    curr->_next=0;/* now null our current elements memory of it */
    destructor(curr->_data);/* destroy the current element */
    free(curr);
    curr=next;/* now set our current element to the lookahead */
  }
  free(list);
  list=0;
}

/***************************************
 * push to the back(append after tail) *
 ***************************************/
void slist_pushb(slist* list,
                 void* data)
{
  if(!list)
    return;

  if(!list->_head)/* check if this list has a head */
  {
    /* if not, lets create its first element */
    (*list)._head=new_slist_elem(data);
  
    /* make sure we keep update the tail and size*/
    (*list)._tail=list->_head;
    (*list)._size=1;
    return;
  }
  
  /* else we rely on a valid tail pointer to append to */
  if(!list->_tail)
    return;

  /* add a new element */
  (*list->_tail)._next=new_slist_elem(data);
  /* make sure we keep update the tail and size*/
  (*list)._tail=list->_tail->_next;
  (*list)._size+=1;
  return;
}

/******************************************
 * push to the front(prepend before head) *
 ******************************************/
void slist_pushf(slist* list,
                 void* data)
{
  slist_elem* new_elem=0;/* pointer to out new element */

  if(!list)
    return;

  /* create a new element and make it point
     to the lists current head */
  new_elem=new_slist_elem(data);
  (*new_elem)._next=list->_head?list->_head:0;

  /* now swap the lists head with the new_elem */
  (*list)._head=new_elem;

  /* if we need to update the tail */
  if(list->_size<1||list->_size==-1)
  {
    (*list)._size=1;
    (*list)._tail=(*list)._head;
  }
  else ++(*list)._size;
}

/*********************************************
 * insert a new element at a specified index *
 *********************************************/
void slist_push_at(slist* list,
                   void* data,
                   int n)
{
  int x=1;
  slist_elem* next=0;
  slist_elem* prev=0;
  slist_elem* _new=0;

  if(!list||n>list->_size||n<1)
    return;

  next=list->_head;
  for(;next->_next,(n-1)!=0;--n)
  {
    prev=next;
    next=next->_next;
  }  
  
  if(!prev)
  {
    slist_pushf(list,data);
    return;
  }

  _new=new_slist_elem(data);
  (*prev)._next=_new;
  (*_new)._next=next;
  (*list)._size+=1;
}

/**********************************
 * pop the tail element (removal) *
 **********************************/
void* slist_popb(slist* list)
{
  slist_elem* prev=0;
  slist_elem* next=0;
  void* data=0;

  if(!list||list->_size<1||!list->_head)
    return 0;


  next=list->_head;
  while(next&&next->_next)
  {
    prev=next;
    next=next->_next;
  }

  if(!prev&&!next)
    return 0;
  if(!prev)
  {
    data=next->_data;
    free(next);
    (*list)._head=0;
    (*list)._tail=0;
    --(*list)._size;
  }
  else
  {
    data=next->_data;
    free(next);
    (*prev)._next=0;
    (*list)._tail=prev;
    --(*list)._size;
  }
  return data;
}

/**********************************
 * pop the head element (removal) *
 **********************************/
void* slist_popf(slist* list)
{
  slist_elem* sle=0;
  void* data=0;

  if(!list||list->_size<1||!list->_head)
    return 0;
  
  sle=list->_head;
  (*list)._head=sle->_next;
  --(*list)._size;
  if(list->_size==1)
    (*list)._tail=list->_head;
  data=sle->_data;
  free(sle);
  return data;
}


/**
 * make an exact copy of a list
 **/
slist* slist_copy(slist* list,
                  void* user_data,
                  void*(*copier)(void*,void*))
{
  slist* ret=0;
  slist_elem* sle=0;

  if(!list||!copier)
    return 0;

  ret=new_slist();
  sle=list->_head;
  for(;sle;sle=sle->_next)
    slist_pushb(ret,copier(user_data,sle->_data));
  return ret;
}
/**
 * make an reverse copy of a list
 **/
slist* slist_rcopy(slist* list,
                   void* user_data,
                   void*(*copier)(void*,void*))
{
  slist* ret=0;
  slist_elem* sle=0;

  if(!list||!copier)
    return 0;

  ret=new_slist();
  sle=list->_head;
  for(;sle;sle=sle->_next)
    slist_pushf(ret,copier(user_data,sle->_data));
  return ret;
}

/**
 * reverse the order of a list
 **/
void slist_reverse(slist* list)
{
  slist_elem* oldhead=0;
  slist_elem* front=0;
  slist_elem* next=0;
  slist_elem* rest=0;

  if(!list||list->_size==1)
    return;

  oldhead=front=list->_head;
  next=front->_next;
  do
  {
    rest=next->_next;
    (*next)._next=front;
    front=next;
    next=rest;
  }while(next);

  (*list)._head=front;
  (*list)._tail=oldhead;
  (*(*list)._tail)._next=0;
}

/**
 * apply a function to each 
 * element of a list.
 **/
void slist_each(slist* list,
                void* user_data,
                void(*function)(void*,void*))
{
  slist_elem* sle=0;
  if(!list||!function)
    return;
  sle=list->_head;
  while(sle)
  {
    function(user_data,sle->_data);
    sle=sle->_next;
  }
}
void slist_each_parallel(slist* a,
                         slist* b,
                         void* user_data,
                         void(*function)(void*,void*))
{
  slist_elem* ale=0;
  slist_elem* ble=0;
  if(!a||!b||!function)
    return;
  ale=a->_head;
  ble=b->_head;
  while(ale&&ble)
  {
    function(user_data,ale->_data);
    function(user_data,ble->_data);
    ale=ale->_next;
    ble=ble->_next;
  }  
}
/**
 * return element at one-based index
 **/
void* slist_at(slist* list,
               int index)
{
  slist_elem* sle=0;
  int x=1;

  if(!list||!list->_size||index>list->_size)
    return 0;

  sle=list->_head;
  for(;sle->_next,x<=list->_size;sle=sle->_next,++x)
    if(x==index)
      return sle->_data;
  return 0;
}
void* slist_remove(slist* list,
                   int index)
{
  slist_elem* prev=0;
  slist_elem* next=0;
  void* data=0;
  int x=1;

  if(!list||!list->_size||index>list->_size)
    return 0;

  next=list->_head;
  for(;next->_next,x<=list->_size;next=next->_next,++x)
  {
    if(x==index)
    {
      data=next->_data;
      if(prev)
      {
        (*prev)._next=next->_next;
        free(next);
        if(!prev->_next)
          (*list)._tail=prev;
      }
      else
      {
        (*list)._head=next->_next;
        (*list)._tail=next->_next;
        free(next);
      }
      --(*list)._size;
      return data;
    }
    prev=next;
  }
  return 0;
}

int slist_indexof(slist* list,
                  void* comp_dat,
                  char(*comp_fn)(void*,void*))
{
  slist_elem* sle=0;
  int i=1;
  if(!list||list->_size<1||!comp_dat||!comp_fn)
    return 0;
  sle=list->_head;
  for(;sle;sle=sle->_next,++i)
    if(comp_fn(sle->_data,comp_dat)==1)
      return i;
  return -1;
}

slist* slist_subseq(slist* list,
                    int from,
                    int to)
{
  slist* ret=0;
  slist_elem* it=0;

  if(!list)
    return ret;
  if(from>list->_size||
     to>list->_size||to<from)
    return ret;

  it=slist_elem_at(list,from);
  if(!it)
    return ret;

  ret=new_slist();
  to=(to+1)-from;
  while(it&&to)
  {
    slist_pushb(ret,it->_data);
    it=it->_next;
    --to;
  }
  return ret;
}

/**PRIVATE**
 * return element at zero-based index
 **/
static slist_elem* slist_elem_at(slist* list,
                                 int index)
{
  slist_elem* sle=0;
  int x=1;

  if(!list||!list->_size||index>list->_size)
    return 0;

  sle=list->_head;
  for(;sle->_next,x<=list->_size;sle=sle->_next,++x)
    if(x==index)
      return sle;
  return 0;
}



//unsigned char slist_remove_at(slist* list,
//                              int index)
//{
//  int x=1;
//  slist_elem* pos=list->_head;
//  slist_elem* temp=0;
// 
//  if(!list||!list->_size||index>list->_size)
//    return 0;
//
//  //iterate to one before element and remove the next element
//  for(;pos->_next,x<list->_size;pos=pos->_next,++x)
//    if(x==index-1)
//    {
//      if(!pos->_next)
//        return 0;
//      if(pos->_next&&pos->_next->_next)
//        temp=pos->_next->_next;
//
//      (*pos)._next->_data=0;
//      free((*pos)._next);
//      (*pos)._next=temp?temp:0;
//      (*list)._size=(*list)._size>0?(*list)._size-=1:0;
//      return 1;
//    }
//  return 0;
//}

//
//slist* slist_split(slist* list,
//                   int index)
//{
//  slist_elem* temp=0;
//  slist* split=0;
//  slist_elem* pos=0;
//  int i=1;
//
//  if(!list||list->_size<1||index>list->_size)
//    return 0;
//  
//  split=new_slist();
//  pos=list->_head;
//  //iterate to one before element and remove the next element
//  for(;pos->_next,i<list->_size;pos=pos->_next,++i)
//    if(i==index-1)
//    {
//      temp=pos->_next;
//      pos->_next=0;
//      split->_head=temp;
//
//      split->_size=list->_size-index;
//      list->_size=index;
//
//      return split;
//    }
//  return 0;
//}