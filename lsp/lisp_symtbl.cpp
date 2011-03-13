#pragma once
#include "stdafx.h"
#include "lisp_symtbl.h"
#include "lisp_eval.h"

/*******************************************************
 * pointer to the current symbol table(global context) *
 *******************************************************/
#define LPSYMTBLSIZE 100
hash_table* lisp_symtbl=0;

/*pjw hash*/
static unsigned int hash_symbol(const void* sym,
                                unsigned int tbl_size)
{
  const char* s=(const char*)sym;
  unsigned int h=0,g=0;
  while(*s)
  {
    h=(h<<4)+*s++;
    if((g=h&(unsigned int)0xf0000000))
      h=(h^(g>>24))^g;
  }return h % tbl_size;
}

char lisp_put_symbol(const char* name,
                     void* atom)
{
  if(!lisp_symtbl)
    lisp_symtbl=new_hash_table(LPSYMTBLSIZE,hash_symbol);
  return hash_table_put(lisp_symtbl,name,(void*)atom);
}
char lisp_get_symbol(const char* name,
                     void** atom)
{
  if(!lisp_symtbl)
    lisp_symtbl=new_hash_table(LPSYMTBLSIZE,hash_symbol);
  *atom=hash_table_get(lisp_symtbl,name);
  return *atom?1:0;
}
char lisp_remove_symbol(const char* name)
{
  if(!lisp_symtbl)
    lisp_symtbl=new_hash_table(LPSYMTBLSIZE,hash_symbol);
  return hash_table_remove(lisp_symtbl,name)?1:0;
}

char lisp_resolve_id(lisp_atom** atom)
{
  int astack_it=0;
  slist* lst=0;
  slist_elem* sle=0;

  if((*atom)->type!=LTID)
    return 0;
  /* check the activation stack for lexical vars */
  astack_it=_activation_stack?_activation_stack->_size:0;
  lst=(slist*)slist_peekfn(_activation_stack,astack_it);
  while(astack_it&&lst)
  {
    sle=lst->_head;
    while(sle&&sle->_next)
      if(strcmp((char*)sle->_data,(char*)(*atom)->data)==0)
      {
        *atom=(lisp_atom*)sle->_next->_data;
        return 1;
      }else sle=sle->_next->_next;
    --astack_it;
    lst=(slist*)slist_peekfn(_activation_stack,astack_it);
  }
  /* check global table, if there is no lex var */
  if(lisp_get_symbol((char*)(*atom)->data,(void**)atom))
    return 1;
  return 0;
}
lisp_atom lp_defun(slist_elem* next)
{
  slist* def_list=new_slist();
  slist_elem* pos=0;
  lisp_atom* fun=0;
  lisp_atom ret;
  char* sym=0;

  /* if this symbol is named then we need to skip an arg*/
  if(ATOM_CAST(next)->type==LTID)
  {
    sym=(char*)ATOM_CAST(next)->data;
    pos=next->_next;
  }else pos=next;

  /* get params */
  if(!pos||ATOM_CAST(pos)->type!=LTLIST)
    LPRETURN_NIL(ret)
  slist_pushb(def_list,(void*)atom_copy(ATOM_CAST(pos)));

  /* get body */
  pos=pos->_next;
  if(!pos||ATOM_CAST(pos)->type!=LTLIST)
    LPRETURN_NIL(ret)
  slist_pushb(def_list,(void*)atom_copy(ATOM_CAST(pos)));

  /* install macro or fn if not a lambda*/
  fun=new_atom(LENORMAL,LTLISPFN,(void*)def_list);
  if(sym&&!lisp_put_symbol(sym,(void*)fun))
    LPRETURN_NIL(ret)

  ret.type=LTLISPFN;
  ret.data=(void*)fun;
  return ret;
}

lisp_atom lp_defvar(slist_elem* next)
{
  lisp_atom ret;
  if(ATOM_CAST(next)->type==LTID&&next->_next)
    if(lisp_put_symbol((const char*)ATOM_CAST(next)->data,next->_next->_data))
      return *ATOM_CAST(next);
  LPRETURN_NIL(ret);
}

lisp_atom lp_let(slist_elem* next)
{
  slist* activation=0; /* pointer to the top of the activation stack */
  slist_elem* sle=0;   /* general iterator */
  lisp_atom ret;       /* return value */

  /* put a new activation record */
  activation=new_slist();

  /* load all parameters */
  if(ATOM_CAST(next)->type!=LTLIST)
    LPRETURN_NIL(ret)
  sle=((slist*)ATOM_CAST(next)->data)->_head;
  while(sle&&sle->_next)
  {
    /* load each argument*/
    if(ATOM_CAST(sle)->type!=LTID)
    {
      slist_destroy(activation,free);
      LPRETURN_NIL(ret)
    }
    slist_pushb(activation,(void*)ATOM_CAST(sle)->data);
    slist_pushb(activation,(void*)ATOM_CAST(sle->_next));
    sle=sle->_next;
  }

  if(!_activation_stack)
    _activation_stack=new_slist();
  slist_pushb(_activation_stack,(void*)activation);

  /* evaluste the lets body */
  next=next->_next;
  if(ATOM_CAST(next)->type!=LTLIST)
  {
    ret=lisp_eval(ATOM_CAST(next),next->_next);
    slist_popf(_activation_stack);
  }
  ret=lisp_eval(ATOM_CAST(next),sle->_next);
  slist_popb(_activation_stack);
  
  return ret;
}
static lisp_atom lp_setf(slist_elem* next)
{
  lisp_atom* atom=0;
  lisp_atom ret;

  /* try and retrieve the symbol */
  if(ATOM_CAST(next)->type==LTID&&next->_next)
  {
    atom=ATOM_CAST(next);
    if(lisp_resolve_id(&atom))
    {
      ret=lisp_eval(ATOM_CAST(next->_next),0);
      (*atom).type=ret.type;
      (*atom).data=ret.data;
      return *atom;
    }
  }
  LPRETURN_NIL(ret)
}

void load_symtbl()
{
  lisp_put_symbol("function",(void*)new_atom(LENORMAL,LTCFNPTR,(void*)new_lisp_cfn(0,2,3,lp_defun)));
  lisp_put_symbol("variable",(void*)new_atom(LENORMAL,LTCFNPTR,(void*)new_lisp_cfn(1,2,CFN_ARGNOCIEL,lp_defvar)));
  lisp_put_symbol("set",(void*)new_atom(LENORMAL,LTCFNPTR,(void*)new_lisp_cfn(0,2,CFN_ARGNOCIEL,lp_setf)));
  lisp_put_symbol("let",(void*)new_atom(LENORMAL,LTCFNPTR,(void*)new_lisp_cfn(0,2,CFN_ARGNOCIEL,lp_let)));
}