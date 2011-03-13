#pragma once
#include "stdafx.h"
#include "lisp_utils.h"
#include "lisp_types.h"
#include "lisp_symtbl.h"
#include <windows.h>

static lisp_atom lp_sleep(slist_elem* next)
{
  lisp_atom ret;
  ret.type=LTTRUE;
  ret.data=(void*)LTTRUE;

  if(ATOM_CAST(next)->type!=LTINT)
  {
    ret.type=LTNIL;
    ret.data=(void*)LTNIL;
    return ret;
  }

  Sleep((DWORD)*(int*)ATOM_CAST(next)->data);
  return ret;
}
static lisp_atom lp_length(slist_elem* next)
{
  lisp_atom ret;
  ret.type=LTINT;
  ret.data=(void*)ALLOC_INT();  
  *(int*)ret.data=atom_length(ATOM_CAST(next));
  return ret;
}

void load_utils()
{
  lisp_put_symbol("sleep",(void*)new_atom(LENORMAL,LTCFNPTR,(void*)new_lisp_cfn(1,1,1,lp_sleep)));
  lisp_put_symbol("length",(void*)new_atom(LENORMAL,LTCFNPTR,(void*)new_lisp_cfn(1,1,1,lp_length)));
}