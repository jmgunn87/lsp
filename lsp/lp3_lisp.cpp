#pragma once
#include "stdafx.h"
#include "lisp_types.h"
#include "lisp_symtbl.h"
#include "lp3_lisp.h"
#include "lisp_eval.h"
#include "lisp_cmp.h"
#include "lisp_cntrl.h"
#include "lisp_math.h"
#include "lisp_io.h"
#include "lisp_access.h"
#include "lisp_utils.h"
#include "lisp_format.h"

static char is_loaded=0; /* have we loaded lisp yet? */
slist* ignore_stack=0;   /* listen stack. */
FILE* lp3_stream=0;      /* our stdout */

/* this is where main processing goes on */
int lp3_process(char* str)
{
  int pos=0;
  slist* lisp_in=0;
  slist_elem* sle=0;

  if(!str)
    return 0;

  lisp_in=lisp_read((const char*)str,&pos);
  if(lisp_in)
  {
    sle=lisp_in->_head;
    while(sle)
    {
      lisp_eval((lisp_atom*)sle->_data,sle->_next);
      sle=sle->_next;
    }
  }return pos;
}

static lisp_atom lp3_if(slist_elem* next)
{
  lisp_atom* def=0;
  lisp_atom ret;
  
  /* push an new listen or ignore scope
     onto lp3's listen stack depending
     on the logical truth of the passed
     atom/expression */
  if(atom_truth(ATOM_CAST(next)))
  {
    lp_stream=lp3_stream;/* now we're listening switch back to lp3's stdout */  
    slist_pushf(ignore_stack,(void*)LP3LISTEN);
    LPRETURN_T(ret)
  }
  /* set lisps stdout to null as we being ignorant */
  lp_stream=0;
  slist_pushf(ignore_stack,(void*)LP3IGNORE);
  LPRETURN_T(ret)
}
static lisp_atom lp3_endif(slist_elem* next)
{
  /* and an ignore or listen state by popping
     the listen stack */
  lisp_atom ret;
  ret.eval_type=LENORMAL;
  ret.type=LTTRUE;
  ret.data=(void*)LTTRUE;
  if((int)slist_popf(ignore_stack)==LP3IGNORE)
    lp_stream=lp3_stream;
  return ret;
}

void load_lp3_lisp()
{
  if(is_loaded)
    return;

  load_types();
  load_symtbl();
  load_math_module();
  load_cmp();
  load_flow();
  load_io();
  load_access();
  load_utils();
  load_format();

  lisp_put_symbol("$if",(void*)new_atom(LENORMAL,LTCFNPTR,(void*)new_lisp_cfn(0,1,1,lp3_if)));
  lisp_put_symbol("$endif",(void*)new_atom(LENORMAL,LTCFNPTR,(void*)new_lisp_cfn(0,0,0,lp3_endif)));
  is_loaded=1;
}