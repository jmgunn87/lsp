#pragma once
#include "stdafx.h"
#include "lisp_eval.h"
#include "lisp_symtbl.h"
/**************************************
 * lexical variables are stored here. *
 * this list must be manually updated *
 * and destroyed by the user          *
 **************************************/
slist* _activation_stack=0;

static lisp_atom lisp_eval_fun(lisp_atom* atom,
                               slist_elem* next)
{
  slist* act=0;         /* functions activation list */
  slist* lst=0;         /* the function list */
  slist_elem* sle=0;    /* general iterator */
  slist_elem* params=0; /* pointer to parameters */
  slist_elem* body=0;   /* pointer to the code body */
  lisp_atom result;     /* function return */

  /* make sure this is formatted correctly*/
  lst=(slist*)atom->data;
  assert(lst->_size==2);
  sle=lst?lst->_head:0;
  if(!sle)
    LPRETURN_NIL(result);

  /* add an activation record for this call */
  if(!_activation_stack)
    _activation_stack=new_slist();
  act=new_slist();

  /* load all parameters */
  params=((slist*)ATOM_CAST(sle)->data)->_head;
  if(params)
    while(params&&next)
    {
      /* load each argument into its activation */
      slist_pushb(act,ATOM_CAST(params)->data);
      slist_pushb(act,(void*)ATOM_CAST(next));

      params=params->_next;
      next=next->_next;
    }
  slist_pushf(_activation_stack,(void*)act);


  /* now eval the body */
  body=((slist*)atom_copy(ATOM_CAST(sle->_next))->data)->_head;
  if(!body)LPRETURN_NIL(result);
  if(ATOM_CAST(body)->type==LTLIST)
    while(body)
    {
      result=lisp_eval(ATOM_CAST(body),body->_next);
      body=body->_next;
    }
  else result=lisp_eval(ATOM_CAST(body),body->_next);

  /* now delete its activation record   
     and  return the evaluated function */
  slist_popf(_activation_stack);
  return result;
}


/* an ugly macro to find the number of arguments a lispfn takes */
#define LSPFNARGC(fn_atom)( ((slist*)ATOM_CAST(((slist*)fn_atom->data)->_head)->data)->_size<0?0:\
                            ((slist*)ATOM_CAST(((slist*)fn_atom->data)->_head)->data)->_size)

lisp_atom lisp_eval(lisp_atom* atom,
                    slist_elem* next)
{
  int lspargc=0;          /* lispfn arg count */
  int argcount=0;         /* lispcfn arg count */
  lisp_cfn* lcfn=0;       /* pointer to cfn */
  lisp_atom ret_atom;     /* return value */
  slist_elem* start=next; /* list start */

  while(atom&&atom->data)
  {
    /* check wether or not there are any 
       special evauluation flags set */
    if(atom->eval_type==LEQUOTE)
    {ret_atom=*atom;return ret_atom;}
    else if(atom->eval_type==LEBQUOTE)
    {ret_atom=*atom;return ret_atom;}

    switch(atom->type)
    {
    case LTID:
      /* resolve any id's */
      ret_atom=*atom;
      if(lisp_resolve_id(&atom))
        continue;
      else return ret_atom;/* just return the symbol */
    case LTLIST:
      /* treat lists as function calls and 
         only eval the first element */
      start=((slist*)atom->data)->_head;
      if(start)
        return lisp_eval((lisp_atom*)start->_data,start->_next);
      else return ret_atom;
    case LTLISPFN:
      start=next;
      while(next)/* eval arguments before a call */
      {
        *ATOM_CAST(next)=lisp_eval(ATOM_CAST(next),0);
        next=next->_next;
        ++argcount;
      }
      /* check the argument count is correct */
      lspargc=LSPFNARGC(atom);
      lspargc<0?lspargc=0:lspargc;
      if(argcount!=lspargc)
        return ret_atom;
      return lisp_eval_fun(atom,start);
    case LTCFNPTR:
      lcfn=(lisp_cfn*)atom->data;
      start=next;
      while(next)/* eval arguments before a call if specified */
      {
        if(lcfn->eval_args)
          *ATOM_CAST(next)=lisp_eval(ATOM_CAST(next),0);
        next=next->_next;
        ++argcount;
      }
      /* check the argument count is correct */
      if(argcount>=lcfn->argc_floor)
        if(lcfn->argc_ciel==CFN_ARGNOCIEL||
           argcount<=lcfn->argc_ciel)
          return lcfn->its_fnptr(start);
      return ret_atom;
    /* by default all other types evaluate to themselves */
    default:
      ret_atom=*atom;
      return ret_atom;
    }
  }
  LPRETURN_NIL(ret_atom);
}

