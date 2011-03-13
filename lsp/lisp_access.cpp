#pragma once 
#include "stdafx.h"
#include "lisp_access.h"
#include "lisp_types.h"
#include "lisp_array.h"
#include "lisp_cmp.h"
#include "lisp_symtbl.h"

static lisp_atom lp_access(lisp_atom* atom,
                           int from,
                           int to)
{
  int diff=0;
  int slen=0;
  char* str=0;
  slist* lst=0;
  lisp_atom ret;

  if(!atom)
    LPRETURN_NIL(ret);

  switch(atom->type)
  {
    case LTSTR:
      str=(char*)atom->data;
      slen=strlen(str);
      if(str&&slen>=to&&slen>=from&&from<=to)/*make sure these indices are correct*/
      {
        ret.type=LTSTR;
        ret.data=(void*)ALLOC_STR(diff+2);
        strncpy((char*)ret.data,(const char*)&((char*)atom->data)[from],diff+1);
        ((char*)ret.data)[diff+1]='\0';
        return ret;
      }
      LPRETURN_NIL(ret);
    case LTLIST:
      lst=slist_subseq((slist*)atom->data,from+1,to+1);
      if(lst)
      {
        if(lst->_size==1)
          return *ATOM_CAST(lst->_head);
        
        ret.type=LTLIST;
        ret.data=(void*)lst;
        return ret;
      }
      LPRETURN_NIL(ret);
    case LTARRAY:
      //la=ATOM_CAST(next);
      //tarr=(lisp_array*)la->data;
      //if(tarr->type!=LTARRAY)
      //{
      //  LPRETURN_NIL(ret);
      //}
      //else
      //{
      //  next=next->_next;

      //  /*traverse the array through each of its dimensions*/
      //  tarr=(lisp_array*)array_at(tarr,index);      
      //  while(tarr&&tarr->type==LTARRAY)
      //  {
      //    next=next->_next;
      //    if(next&&ATOM_CAST(next)->type==LTINT)
      //      index=*(int*)ATOM_CAST(next)->data;
      //    else LPRETURN_NIL(ret);
      //    tarr=(lisp_array*)array_at(tarr,index);
      //  }
      //  /*this will either be a terminal dimension (iee not LTARRAY) or null/nil*/
      //  if(tarr&&next&&next->_next)
      //    if(ATOM_CAST(next->_next)->type==LTINT&&
      //       ATOM_CAST(next->_next)->data!=0)
      //    {
      //      ret.type=tarr->type;
      //      ret.data=array_at(tarr,*(int*)ATOM_CAST(next->_next)->data);
      //      return ret;
      //    }
      //}
      //LPRETURN_NIL(ret);
    default:
      break;
  }
  LPRETURN_NIL(ret);
}
static lisp_atom lp_at(slist_elem* next)
{
  lisp_atom ret;
  if(ATOM_CAST(next->_next)->type!=LTINT)
    LPRETURN_NIL(ret);
  return lp_access(ATOM_CAST(next),
                   *(int*)ATOM_CAST(next->_next)->data,
                   *(int*)ATOM_CAST(next->_next)->data);
}
static lisp_atom lp_between(slist_elem* next)
{
  lisp_atom ret;
  if(ATOM_CAST(next->_next)->type!=LTINT||
     ATOM_CAST(next->_next->_next)->type!=LTINT)
    LPRETURN_NIL(ret);
  return lp_access(ATOM_CAST(next),
                   *(int*)ATOM_CAST(next->_next)->data,
                   *(int*)ATOM_CAST(next->_next->_next)->data);
}
static lisp_atom lp_first(slist_elem* next)
{
  return lp_access(ATOM_CAST(next),0,0);
}
static lisp_atom lp_last(slist_elem* next)
{
  lisp_atom ret;
  if(ATOM_CAST(next->_next)->type!=LTINT)
    LPRETURN_NIL(ret);
  return lp_access(ATOM_CAST(next),0,*(int*)ATOM_CAST(next->_next)->data);
}
static lisp_atom lp_index(slist_elem* next)
{
  lisp_atom ret;
  if(ATOM_CAST(next->_next)->type!=LTINT)
    LPRETURN_NIL(ret);
  return lp_access(ATOM_CAST(next),0,*(int*)ATOM_CAST(next->_next)->data);
}
static lisp_atom lp_from(slist_elem* next)
{
  lisp_atom ret;
  if(ATOM_CAST(next->_next)->type!=LTINT)
    LPRETURN_NIL(ret);
  return lp_access(ATOM_CAST(next),0,*(int*)ATOM_CAST(next->_next)->data);
}
static lisp_atom lp_to(slist_elem* next)
{
  lisp_atom ret;
  if(ATOM_CAST(next->_next)->type!=LTINT)
    LPRETURN_NIL(ret);
  return lp_access(ATOM_CAST(next),0,*(int*)ATOM_CAST(next->_next)->data);
}
void load_access()
{
  lisp_put_symbol("at",(void*)new_atom(LENORMAL,LTCFNPTR,(void*)new_lisp_cfn(1,2,CFN_ARGNOCIEL,lp_at)));
  lisp_put_symbol("between",(void*)new_atom(LENORMAL,LTCFNPTR,(void*)new_lisp_cfn(1,3,3,lp_between)));
  lisp_put_symbol("first",(void*)new_atom(LENORMAL,LTCFNPTR,(void*)new_lisp_cfn(1,1,1,lp_first)));
  //lisp_put_symbol("last",(void*)new_atom(LENORMAL,LTCFNPTR,(void*)new_lisp_cfn(1,1,1,lp_first)));
  lisp_put_symbol("index",(void*)new_atom(LENORMAL,LTCFNPTR,(void*)new_lisp_cfn(1,2,2,lp_index)));
  //lisp_put_symbol("from",(void*)new_atom(LENORMAL,LTCFNPTR,(void*)new_lisp_cfn(1,2,2,lp_from)));
  lisp_put_symbol("to",(void*)new_atom(LENORMAL,LTCFNPTR,(void*)new_lisp_cfn(1,2,2,lp_to)));
}
