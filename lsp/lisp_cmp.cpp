#pragma once
#include "stdafx.h"
#include "lisp_cmp.h"
#include "lisp_types.h"
#include "lisp_symtbl.h"

typedef enum _CMP_OP
{
  CMP_EQ,
  CMP_NEQ,
  CMP_LT,
  CMP_LEQ,
  CMP_GT,
  CMP_GEQ

} CMP_OP;

static char cmp_int(int lval,
                    int rval,
                    CMP_OP op)
{
  switch(op)
  {
    case CMP_EQ:  return lval==rval? 1:0;
    case CMP_NEQ: return lval!=rval? 1:0;
    case CMP_LT:  return lval<rval?  1:0;
    case CMP_LEQ: return lval<=rval? 1:0;
    case CMP_GT:  return lval>rval?  1:0;
    case CMP_GEQ: return lval>=rval? 1:0;
    default:      return 0;
  }
  return 0;
}

static char cmp_float(float lval,
                      float rval,
                      CMP_OP op)
{
  switch(op)
  {
    case CMP_EQ:  return lval==rval?1:0;
    case CMP_NEQ: return lval!=rval?1:0;
    case CMP_LT:  return lval<rval?1:0;
    case CMP_LEQ: return lval<=rval?1:0;
    case CMP_GT:  return lval>rval?1:0;
    case CMP_GEQ: return lval>=rval?1:0;
    default:      return 0;
  }
  return 0;
}

static char cmp_str(char* lval,
                    char* rval,
                    CMP_OP op)
{
  char tres=0;

  if(!lval||!rval)
    return 0;

  tres=strcmp((const char*)lval,(const char*)rval)==0;
  if(op==CMP_EQ)
    return tres;
  if(op==CMP_NEQ)
    return !tres;
  return 0;
}

//static char cmp_list(slist* lval,
//                     slist* rval,
//                     CMP_OP op)
//{
//  slist_elem* lle=0;
//  slist_elem* rle=0;
//
//  if(!lval||lval->_size<1||
//     !rval||rval->_size<1)
//    return 0;
//
//  //do a quick rejection/acceptance test on thier sizes
//  if(lval->_size==rval->_size)
//    return 1;
//
//  lle=lval->_head;
//  rle=rval->_head;
//
//  while(lle&&rle)
//  {
//  }
//}


char atom_match(void* a,
                void* b)
{
  slist* alst=0;
  slist* blst=0;
  slist_elem* ale=0;
  slist_elem* ble=0;
  lisp_atom* atom_a=0;
  lisp_atom* atom_b=0;

  if(!a||!b)
    return 0;

  atom_a=(lisp_atom*)a;
  atom_b=(lisp_atom*)b;

  if(atom_a->type!=atom_b->type)
    return 0;

  switch(atom_a->type)
  {
  case LTID:
  case LTSTR:
    return strcmp((char*)atom_a->data,
                  (char*)atom_b->data)==0?1:0;
  case LTINT:
    return *(int*)atom_a->data==
           *(int*)atom_b->data?
           1:0;
  case LTFLOAT:
    return *(float*)atom_a->data==
           *(float*)atom_b->data?
           1:0;
  case LTLIST:
    alst=(slist*)atom_a->data;
    blst=(slist*)atom_b->data;
    if(alst->_size!=blst->_size)
      return 0;

    /***********************************
     * traverse both lists in parallel *
     * break out on the first mismatch *
     ***********************************/
    ale=alst->_head;
    ble=blst->_head;
    while(ale&&ble)
    {
      if(!atom_match((void*)ale->_data,
                     (void*)ble->_data))
        return 0;

      ale=ale->_next;
      ble=ble->_next;
    }
    return 1;
  case LTTRUE:
  case LTNIL:
  default:
    return 1;
  }
}

static lisp_atom lp_cmp(slist_elem* next,
                        CMP_OP op)
{
  lisp_atom* lval=ATOM_CAST(next);
  lisp_atom* rval=ATOM_CAST(next->_next);
  lisp_atom ret;
  ret.type=LTNIL;

  /***************************************
   * evaluate all expressions            *
   ***************************************/
  next=next->_next;
  while(1) 
  {
    if(lval->type!=rval->type)
      return ret;
    
    switch(lval->type)
    {
    case LTSTR:
    case LTID:
      if(!cmp_str((char*)lval->data,(char*)rval->data,op))
        return ret;
      break;
    case LTFLOAT:
      if(!cmp_float(*(float*)lval->data,*(float*)rval->data,op))
      return ret;
      break;
    case LTINT:
      if(!cmp_int(*(int*)lval->data,*(int*)rval->data,op))
      return ret;
      break;
    case LTLISPFN:
    case LTLISPMACRO:
    case LTLIST:
    default:
      return ret;
      break;
    }
    lval=rval;
    if(next->_next)
    {
      rval=ATOM_CAST(next->_next);
      next=next->_next;
    }else break;
  }
  ret.type=LTTRUE;
  return ret;
}
static lisp_atom lp_eq(slist_elem* next){return lp_cmp(next,CMP_EQ);}
static lisp_atom lp_neq(slist_elem* next){return lp_cmp(next,CMP_NEQ);}
static lisp_atom lp_lt(slist_elem* next){return lp_cmp(next,CMP_LT);}
static lisp_atom lp_leq(slist_elem* next){return lp_cmp(next,CMP_LEQ);}
static lisp_atom lp_gt(slist_elem* next){return lp_cmp(next,CMP_GT);}
static lisp_atom lp_geq(slist_elem* next){return lp_cmp(next,CMP_GEQ);}

void load_cmp()
{
  lisp_put_symbol("=",(void*)new_atom(LENORMAL,LTCFNPTR,(void*)new_lisp_cfn(1,0,CFN_ARGNOCIEL,lp_eq)));
  lisp_put_symbol("equal",(void*)new_atom(LENORMAL,LTCFNPTR,(void*)new_lisp_cfn(1,0,CFN_ARGNOCIEL,lp_eq)));
  lisp_put_symbol("~=",(void*)new_atom(LENORMAL,LTCFNPTR,(void*)new_lisp_cfn(1,0,CFN_ARGNOCIEL,lp_neq)));
  lisp_put_symbol("unequal",(void*)new_atom(LENORMAL,LTCFNPTR,(void*)new_lisp_cfn(1,0,CFN_ARGNOCIEL,lp_neq)));
  lisp_put_symbol("<",(void*)new_atom(LENORMAL,LTCFNPTR,(void*)new_lisp_cfn(1,0,CFN_ARGNOCIEL,lp_lt)));
  lisp_put_symbol("less",(void*)new_atom(LENORMAL,LTCFNPTR,(void*)new_lisp_cfn(1,0,CFN_ARGNOCIEL,lp_lt)));
  lisp_put_symbol("<=",(void*)new_atom(LENORMAL,LTCFNPTR,(void*)new_lisp_cfn(1,0,CFN_ARGNOCIEL,lp_leq)));
  lisp_put_symbol("less-or-equal",(void*)new_atom(LENORMAL,LTCFNPTR,(void*)new_lisp_cfn(1,0,CFN_ARGNOCIEL,lp_leq)));
  lisp_put_symbol(">",(void*)new_atom(LENORMAL,LTCFNPTR,(void*)new_lisp_cfn(1,0,CFN_ARGNOCIEL,lp_gt)));
  lisp_put_symbol("greater",(void*)new_atom(LENORMAL,LTCFNPTR,(void*)new_lisp_cfn(1,0,CFN_ARGNOCIEL,lp_gt)));
  lisp_put_symbol(">=",(void*)new_atom(LENORMAL,LTCFNPTR,(void*)new_lisp_cfn(1,0,CFN_ARGNOCIEL,lp_geq)));
  lisp_put_symbol("greater-or-equal",(void*)new_atom(LENORMAL,LTCFNPTR,(void*)new_lisp_cfn(1,0,CFN_ARGNOCIEL,lp_geq)));
}