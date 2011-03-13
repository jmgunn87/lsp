#pragma once 
#include "stdafx.h"
#include "lisp_types.h"
#include "lisp_symtbl.h"

typedef enum _MATH_OP
{
  LM_MULT=1,
  LM_ADD=2,
  LM_DIV=3,
  LM_SUB=4,
  LM_MOD=5

} MATH_OP;

static lisp_atom lp_math_eval(slist_elem* next,
                              MATH_OP _operator)
{
  lisp_atom ret;
  ret.type=LTINT;
  ret.data=0;
  int operand=0;
  int* sum=(int*)malloc(sizeof(int));
  *sum=0;
  
  while(next)
  {   
    if(ATOM_CAST(next)->type!=LTINT)
      break;
    else operand=*(int*)ATOM_CAST(next)->data;
    switch(_operator)
    {
      case LM_MULT:
        if(!*sum)(*sum)=operand;
        else (*sum)*=operand;
        break;
      case LM_ADD:
        (*sum)+=operand;
        break;
      case LM_DIV:
        if(!*sum)(*sum)=operand;
        else (*sum)||operand!=0?(*sum)/=operand:0;/* catch div by zero */
        break;
      case LM_SUB:
        if(!*sum)(*sum)=operand;
        else (*sum)-=operand;
        break;
      case LM_MOD:
        if(!*sum)(*sum)=operand;
        else (*sum)%=operand;
        break;
      default:
        break;
    }
    next=next->_next;
  }
  ret.data=(void*)sum;
  return ret;
}

static lisp_atom lp_mult(slist_elem* next){return lp_math_eval(next,LM_MULT);}
static lisp_atom lp_add(slist_elem* next){return lp_math_eval(next,LM_ADD);}
static lisp_atom lp_div(slist_elem* next){return lp_math_eval(next,LM_DIV);}
static lisp_atom lp_sub(slist_elem* next){return lp_math_eval(next,LM_SUB);}
static lisp_atom lp_mod(slist_elem* next){return lp_math_eval(next,LM_MOD);}

void load_math_module()
{
  lisp_put_symbol("*",(void*)new_atom(LENORMAL,LTCFNPTR,(void*)new_lisp_cfn(1,0,CFN_ARGNOCIEL,lp_mult)));
  lisp_put_symbol("+",(void*)new_atom(LENORMAL,LTCFNPTR,(void*)new_lisp_cfn(1,0,CFN_ARGNOCIEL,lp_add)));
  lisp_put_symbol("/",(void*)new_atom(LENORMAL,LTCFNPTR,(void*)new_lisp_cfn(1,0,CFN_ARGNOCIEL,lp_div)));
  lisp_put_symbol("-",(void*)new_atom(LENORMAL,LTCFNPTR,(void*)new_lisp_cfn(1,0,CFN_ARGNOCIEL,lp_sub)));
  lisp_put_symbol("%",(void*)new_atom(LENORMAL,LTCFNPTR,(void*)new_lisp_cfn(1,0,CFN_ARGNOCIEL,lp_mod)));
}