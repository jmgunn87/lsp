#pragma once
#include "stdafx.h"
#include "lisp_types.h"
#include "lisp_symtbl.h"
#include "lisp_conv.h"

lisp_atom lp_as(slist_elem* next)
{
  char* keyword=0;
  lisp_atom ret;

  if(ATOM_CAST(next->_next)->type!=LTID)
    LPRETURN_NIL(ret);

  keyword=(char*)ATOM_CAST(next->_next)->data;
  if(!keyword)
    LPRETURN_NIL(ret);

  if(strcmp(keyword,"int")==0)
    (*ATOM_CAST(next)).type=LTINT;
  else if(strcmp(keyword,"float")==0)
    (*ATOM_CAST(next)).type=LTFLOAT;
  else if(strcmp(keyword,"string")==0)
    (*ATOM_CAST(next)).type=LTSTR;
  else if(strcmp(keyword,"list")==0)
    (*ATOM_CAST(next)).type=LTLIST;
  else if(strcmp(keyword,"array")==0)
    (*ATOM_CAST(next)).type=LTARRAY;
  else if(strcmp(keyword,"function")==0)
    (*ATOM_CAST(next)).type=LTLISPFN;
  else if(strcmp(keyword,"macro")==0)
    (*ATOM_CAST(next)).type=LTLISPMACRO;
  else if(strcmp(keyword,"fnptr")==0)
    (*ATOM_CAST(next)).type=LTCFNPTR;
  else if(strcmp(keyword,"id")==0)
    (*ATOM_CAST(next)).type=LTID;
  else LPRETURN_NIL(ret)

  return *ATOM_CAST(next);
}

lisp_atom lp_of(slist_elem* next)
{
  char* keyword=0;
  lisp_atom ret;
  LISP_TYPE type=LTNIL;

  if(ATOM_CAST(next->_next)->type!=LTID)
    LPRETURN_NIL(ret);

  keyword=(char*)ATOM_CAST(next->_next)->data;
  if(!keyword)
    LPRETURN_NIL(ret);

  if(strcmp(keyword,"int")==0)
    type=LTINT;
  else if(strcmp(keyword,"float")==0)
    type=LTFLOAT;
  else if(strcmp(keyword,"string")==0)
    type=LTSTR;
  else if(strcmp(keyword,"list")==0)
    type=LTLIST;
  else if(strcmp(keyword,"array")==0)
    type=LTARRAY;
  else if(strcmp(keyword,"function")==0)
    type=LTLISPFN;
  else if(strcmp(keyword,"macro")==0)
    type=LTLISPMACRO;
  else if(strcmp(keyword,"fnptr")==0)
    type=LTCFNPTR;
  else if(strcmp(keyword,"id")==0)
    type=LTID;

  if(ATOM_CAST(next)->type==type)
    LPRETURN_T(ret);
  LPRETURN_NIL(ret);
}

void load_conv()
{
  lisp_put_symbol("as",(void*)new_atom(LENORMAL,LTCFNPTR,(void*)new_lisp_cfn(1,2,2,lp_as)));
  lisp_put_symbol("of",(void*)new_atom(LENORMAL,LTCFNPTR,(void*)new_lisp_cfn(1,2,2,lp_of)));
}