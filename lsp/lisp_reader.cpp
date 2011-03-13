#pragma once
#include "stdafx.h"
#include "lisp_reader.h"

char terminator_macros[256];

slist* lisp_read(const char* str,
                 int* pos)
{
  LEVAL_TYPE curr_prefix=LENORMAL;
  slist* stack=new_slist();
  lisp_atom* tatom=0;
  void* tmp=0;
  int strpos=0;
  int sublen=0;
  int open=0;
  char is_float=0;

  /*initialize the stack with a root list entry.
    this will be stripped when we return */
  lisp_atom stack_start;
  stack_start.eval_type=LENORMAL;
  stack_start.type=LTLIST;
  stack_start.data=(void*)new_slist();
  slist_pushf(stack,(void*)&stack_start);

  for(;strpos<strlen(str);++strpos)
    switch(str[strpos])
    {
    case '~':
      if(open)
        return 0;
      if(pos)
        *pos=strpos;
      tatom=(lisp_atom*)slist_popf(stack);
      slist_destroy(stack,free);
      return (slist*)tatom->data;
    case ' ':case '\n':case '\t':case '\r':case '\v':
      break;
    case '\'':curr_prefix=LEQUOTE;
      break;
    case '`':curr_prefix=LEBQUOTE;
      break;
    case ',':curr_prefix=LECOMMA;
      break;
    case '@':
      if(curr_prefix==LECOMMA)
        curr_prefix=LECOMMA_AT;
      else curr_prefix=LEAT;
      break;
    case '(':
      ++open;
      slist_pushf(stack,(void*)new_atom(curr_prefix,LTLIST,(void*)new_slist()));
      curr_prefix=LENORMAL;
      break;
    case ')':
      --open;
      if(!stack)
        return 0;
      tmp=slist_popf(stack);
      if(!slist_peekf(stack)||((lisp_atom*)slist_peekf(stack))->type!=LTLIST)
        return 0;
      slist_pushb((slist*)((lisp_atom*)slist_peekf(stack))->data,tmp);
      break;
    case '\"':
      /* loop through till we see another quote */
      ++strpos;
      sublen=0;
      for(;str[strpos]!='\"';++strpos,++sublen)
      {
        if(str[strpos]=='\0'||strpos==strlen(str))
          return 0;
        if(str[strpos]=='\\')
        {++strpos;++sublen;}
      }
      /* now copy the string*/
      tatom=new_atom(curr_prefix,LTSTR,malloc(sublen+1));
      curr_prefix=LENORMAL;
      strncpy((char*)tatom->data,&str[strpos-sublen],sublen);
      ((char*)tatom->data)[sublen]='\0';
      slist_pushb((slist*)((lisp_atom*)slist_peekf(stack))->data,(void*)tatom);
      break;
    default:
      if(!isdigit(str[strpos]))
      {
        /* loop through till we see some wspace */
        sublen=0;
        for(;str[strpos]!=' '&&
          str[strpos]!=')'&&
          str[strpos]!='('&&
          str[strpos]!='\"'&&
          str[strpos]!='\''&&
          str[strpos]!='`'&&
          str[strpos]!=','&&
          str[strpos]!='@'&&
          str[strpos]!='\0';++strpos,++sublen)
          ;
        /* now copy the string*/
        tatom=new_atom(curr_prefix,LTID,malloc(sublen+1));
        curr_prefix=LENORMAL;
        strncpy((char*)tatom->data,&str[strpos-sublen],sublen);
        ((char*)tatom->data)[sublen]='\0';
        slist_pushb((slist*)((lisp_atom*)slist_peekf(stack))->data,tatom);
      }
      else
      {
        sublen=0;
        for(;isdigit(str[strpos])||str[strpos]=='.';++sublen,++strpos)
          if(str[strpos]=='.')
            is_float=true;

        if(is_float)
        {
          tatom=new_atom(curr_prefix,LTFLOAT,malloc(sizeof(float)));
          curr_prefix=LENORMAL;
          *(float*)tatom->data=atof((const char*)&str[strpos-sublen]);
          slist_pushb((slist*)((lisp_atom*)slist_peekf(stack))->data,(void*)tatom);
        }
        else
        {
          tatom=new_atom(curr_prefix,LTINT,malloc(sizeof(int)));
          curr_prefix=LENORMAL;
          *(int*)tatom->data=atoi(&str[strpos-sublen]);
          slist_pushb((slist*)((lisp_atom*)slist_peekf(stack))->data,(void*)tatom);
        }
      }
      --strpos;
      break;
    }
  if(open)
    return 0;
  if(pos)
    *pos=strpos;
  tatom=(lisp_atom*)slist_popf(stack);
  slist_destroy(stack,free);
  return (slist*)tatom->data;
}