#pragma once
#include "stdafx.h"
#include "lisp_io.h"
#include "lisp_types.h"
#include "lisp_symtbl.h"

/* pointer to the current output stream */
FILE* lp_stream=0;

static void print_atom(void* ud,
                       void* atom_in)
{
  lisp_atom* atom=(lisp_atom*)atom_in;
  if(!lp_stream)
    return;
  while(atom)
    switch(atom->type)
    {
    case LTTRUE:      fprintf(lp_stream,"T");                      return;
    case LTNIL:       fprintf(lp_stream,"NIL");                    return;
    case LTID:        fprintf(lp_stream,"%s",atom->data);          return;
    case LTINT:       fprintf(lp_stream,"%d",*(int*)atom->data);   return;
    case LTFLOAT:     fprintf(lp_stream,"%f",*(float*)atom->data); return;
    case LTSTR:       fprintf(lp_stream,"%s",atom->data);          return;
    case LTLIST:      fprintf(lp_stream,"("); 
                      slist_each((slist*)atom->data,0,print_atom);
                      fprintf(lp_stream,")");                       return;
    case LTCFNPTR:    fprintf(lp_stream,"0x%X",atom->data);        return;
    case LTARRAY:                                                   return;
    case LTLISPFN:    
      if(((slist*)atom->data)->_size>2)
        fprintf(stdout,"LISPFN:%s",
             (char*)((slist*)atom->data)->_head->_data);
      else fprintf(stdout,"LAMBDA /\\");                            return;
    case LTLISPMACRO:                                               return;
    default:
      return;
    }
}
static lisp_atom lp_echo(slist_elem* next)
{
  lisp_atom ret;
  while(next)
  {
    print_atom(0,(void*)ATOM_CAST(next));
    next=next->_next;
  }
  LPRETURN_T(ret)
}

static lisp_atom lp_set_stream(slist_elem* next)
{
  lisp_atom ret;
  
  if(ATOM_CAST(next)->type==LTSTR&&
     ATOM_CAST(next->_next)->type==LTSTR)
  {
    /* if we already have an active stream, close it*/
    if(lp_stream)
      fclose(lp_stream);
    lp_stream=fopen((const char*)ATOM_CAST(next)->data,
                    (const char*)ATOM_CAST(next->_next)->data);
    if(lp_stream)
      LPRETURN_T(ret)
  }
  LPRETURN_NIL(ret)
}
static lisp_atom lp_close_stream(slist_elem* next)
{
  lisp_atom ret;
  if(lp_stream)
    if(fclose(lp_stream))
      LPRETURN_T(ret)
  LPRETURN_NIL(ret)
}

void load_io()
{
  lisp_put_symbol("write",(void*)new_atom(LENORMAL,LTCFNPTR,(void*)new_lisp_cfn(1,0,CFN_ARGNOCIEL,lp_echo)));
  lisp_put_symbol("open",(void*)new_atom(LENORMAL,LTCFNPTR,(void*)new_lisp_cfn(1,2,2,lp_set_stream)));
  lisp_put_symbol("close",(void*)new_atom(LENORMAL,LTCFNPTR,(void*)new_lisp_cfn(1,0,0,lp_close_stream)));
}