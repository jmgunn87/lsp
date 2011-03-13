#pragma once
#include "stdafx.h"
#include "lisp_types.h"
#include "lisp_symtbl.h"

lisp_atom* new_atom(LEVAL_TYPE et,
                    LISP_TYPE type,
                    void* data)
{
  lisp_atom* la=(lisp_atom*)malloc(sizeof(lisp_atom));
  la->eval_type=et;
  la->type=type;
  la->data=data;
  return la;
}

char atom_truth(lisp_atom* atom)
{
  if(!atom)
    return 0;
  while(1)
    switch(atom->type)
    {
      case LTID:
        if(lisp_resolve_id(&atom))
          continue;
        return 0;
      case LTFLOAT:
        if(!atom->data||*(float*)atom->data<1)
          return 0;
        return 1;
      case LTINT:
        if(!atom->data||*(int*)atom->data<1)
          return 0;
        return 1;
      case LTSTR:
      case LTLISPFN:
      case LTLISPMACRO:
        return atom->data?1:0;
      case LTLIST:
        if(!atom->data||((slist*)atom->data)->_size<1)
          return 0;
        return 1;
      case LTTRUE:
        return 1;
      case LTNIL:
      default:
        return 0;
    }
}

lisp_atom* atom_copy(lisp_atom* atom)
{
  slist* tlist=0;
  slist_elem* tle=0;
  lisp_atom* copy=0;

  copy=new_atom(LENORMAL,LTNIL,(void*)LTNIL);
  if(!atom)
    return copy;

  (*copy).eval_type=atom->eval_type;
  switch(atom->type)
  {
  case LTTRUE:
    (*copy).type=LTTRUE;
    (*copy).data=(void*)LTTRUE;
    return copy;
  case LTID:
    (*copy).type=LTID;
    (*copy).data=malloc(strlen((char*)atom->data)+1);
    ((char*)(*copy).data)[strlen((char*)atom->data)]='\0';
    strcpy((char*)(*copy).data,(char*)atom->data);
    return copy;
  case LTINT:
    (*copy).type=LTINT;
    (*copy).data=malloc(sizeof(int));
    *(int*)(*copy).data=*(int*)atom->data;
    return copy;
  case LTFLOAT:
    (*copy).type=LTFLOAT;
    (*copy).data=malloc(sizeof(float));
    *(float*)(*copy).data=*(float*)atom->data;
    return copy;
  case LTSTR:
    (*copy).type=LTSTR;
    (*copy).data=malloc(strlen((char*)atom->data)+1);
    ((char*)(*copy).data)[strlen((char*)atom->data)]='\0';
    strcpy((char*)(*copy).data,(char*)atom->data);
    return copy;
  case LTLIST:
    (*copy).type=LTLIST;
    (*copy).data=(void*)new_slist();
    tlist=(slist*)atom->data;
    tle=tlist->_head;
    while(tle)
    {
      slist_pushb((slist*)(*copy).data,
        (void*)atom_copy((lisp_atom*)tle->_data));
      tle=tle->_next;
    }
    return copy;
  case LTLISPFN:
    (*copy).type=LTLISPFN;
    (*copy).data=(void*)new_slist();
    return copy;
  case LTLISPMACRO:
    (*copy).type=LTLISPMACRO;
    (*copy).data=(void*)new_slist();
    return copy;
  case LTARRAY:
    (*copy).type=LTARRAY;
    (*copy).data=0;//call a specialized array copier
    return copy;
  case LTCFNPTR:
    (*copy).type=LTCFNPTR;
    (*copy).data=(void*)new_lisp_cfn(
      ((lisp_cfn*)atom->data)->eval_args,
      ((lisp_cfn*)atom->data)->argc_floor,
      ((lisp_cfn*)atom->data)->argc_ciel,
      ((lisp_cfn*)atom->data)->its_fnptr);
    return copy;
  case LTNIL:
  default:
    return copy;
  }
}

void atom_destroy(void* patom)
{
  int esz=0;
  slist* tlist=0;
  slist_elem* tle=0;
  lisp_atom* atom=0;
  
  if(!patom)
    return;

  atom=(lisp_atom*)patom;
  while(1)
    switch(atom->type)
    {
    //case LTID:
    //  (*copy).type=LTID;
    //  (*copy).data=malloc(strlen((char*)atom->data)+1);
    //  ((char*)(*copy).data)[strlen((char*)atom->data)]='\0';
    //  strcpy((char*)(*copy).data,(char*)atom->data);
    //  return copy;
    case LTINT:
    case LTFLOAT:
    case LTSTR:   
      free((*atom).data); 
      break;
    case LTARRAY:
      esz=lisp_array_elem_size((lisp_array*)(*atom).data);
      if(esz)
      {
        ;//call a specialised array destructor.
      }
      free((*atom).data);
      break;
    case LTLIST:
      tlist=(slist*)atom->data;
      tle=tlist->_head;
      while(tle)
      {
        atom_destroy((lisp_atom*)tle->_data);
        tle=tle->_next;
      }
      slist_destroy(tlist,atom_destroy);
      break;
    case LTLISPFN:
    case LTLISPMACRO:
    case LTCFNPTR:
      (*atom).data=0;
      break;
    default:
      break;
    }
  free(atom);
  atom=0;
}

int atom_length(lisp_atom* atom)
{
  if(!atom)
    return -1;
  switch(atom->type)
  {
  case LTSTR:   return strlen((char*)atom->data);
  case LTLIST:  return ((slist*)atom->data)->_size;
  case LTARRAY: return ((lisp_array*)atom->data)->length;
  default:
    break;
  }
  return -1;
}

lisp_cfn* new_lisp_cfn(char eval,
                       int floor,
                       int ciel,
                       lisp_atom(*fnptr)(slist_elem*))
{
  lisp_cfn* lcf=(lisp_cfn*)malloc(sizeof(lisp_cfn));
  lcf->eval_args=eval;
  lcf->argc_floor=floor;
  lcf->argc_ciel=ciel;
  lcf->its_fnptr=fnptr;
  return lcf;
}

lisp_array* new_lisp_array(LISP_TYPE type,
                           int length,
                           void* data)
{
  lisp_array* la=(lisp_array*)malloc(sizeof(lisp_array));
  la->type=type;
  la->length=length;
  la->data=data;
  return la;
}

int lisp_array_elem_size(lisp_array* la)
{
  if(!la)
    return 0;

  switch(la->type)
  {
    case LTTRUE:   return sizeof(long);
    case LTNIL:    return sizeof(long);
    case LTINT:    return sizeof(int);
    case LTFLOAT:  return sizeof(float);
    case LTSTR:    return sizeof(char*);
    case LTID:     return sizeof(char*);
    case LTARRAY:  return sizeof(lisp_array*);
    case LTLIST:   return sizeof(slist*);
    case LTCFNPTR: return sizeof(void*);
    default:
      return 0;
  }
}
void load_types()
{
  /*lisp_put_symbol("constant",(void*)new_atom(LTCFNPTR,0,0);
  lisp_put_symbol("string",(void*)new_atom(LTCFNPTR,0,0);
  lisp_put_symbol("int",(void*)new_atom(LTCFNPTR,0,0);
  lisp_put_symbol("float",(void*)new_atom(LTCFNPTR,0,0);
  lisp_put_symbol("id",(void*)new_atom(LTCFNPTR,0,0);
  lisp_put_symbol("list",(void*)new_atom(LTCFNPTR,0,0);
  lisp_put_symbol("array",(void*)new_atom(LTCFNPTR,0,0);*/
}