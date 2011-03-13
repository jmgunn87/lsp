#pragma once
#include "stdafx.h"
#include "lisp_array.h"

void array_type_depth(const lisp_array* larray,
                      LISP_TYPE* type,
                      int* depth)
{
  if(!larray)
    return;
  if(depth)
    *depth=1;
  while(larray->type==LTARRAY)
  {
    larray=(const lisp_array*)larray->data;
    if(depth)
      ++*depth;
  }
  if(type)
    *type=larray->type;
}

int array_depth_step(const lisp_array* larray,
                     int depth,
                     int width)
{
  int step=0;
  if(!larray)
    return -1;

  step=larray->length;
  while(larray->type==LTARRAY&&depth!=0)
  {
    larray=(const lisp_array*)larray->data;
    step+=larray->length*4;
    --depth;
  }return larray->length*width;
}

void* array_at(lisp_array* la,
               int index)
{
  if(!la||index>la->length)
    return 0;
  switch(la->type)
  {
    case LTARRAY:     return (void*)((lisp_array**)la->data)[index];
    case LTID:
    case LTSTR:       return (void*)&((char**)la->data)[index];
    case LTINT:       return (void*)&((int*)la->data)[index];
    case LTFLOAT:     return (void*)&((float*)la->data)[index];    
    case LTLIST:      return (void*)((slist**)la->data)[index];
    case LTLISPFN:    return (void*)((slist**)la->data)[index];
    case LTLISPMACRO: return (void*)((slist**)la->data)[index];
    case LTCFNPTR:    return (void*)((int(**)(slist_elem*))la->data)[index];
    default:
      break;
  }return 0;
}