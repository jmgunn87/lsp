#pragma once
#include "stdafx.h"
#include "lisp_types.h"
#include "lisp_format.h"
#include "lisp_symtbl.h"

#define EXTEND_BUFFER(buff,curr,plus)\
{\
  curr+=plus;\
  buff=(char*)realloc((void*)buff,curr);\
  buff[curr-1]='\0';\
}
#define WRITE_BUFFER_C(buff,sym,pos,size)\
{\
  if(pos+1>=size)\
    EXTEND_BUFFER(buff,size,size)\
  buff[pos]=sym;\
  buff[pos+1]='\0';\
  ++pos;\
}
#define WRITE_BUFFER_S(buff,str,pos,size)\
{\
  if(pos+strlen(str)>=size)\
    EXTEND_BUFFER(buff,size,pos+strlen(str))\
  strcpy(&buff[pos],str);\
  pos+=strlen(str);\
  buff[pos+1]='\0';\
}

/* super function! 
   i should really break this up*/
static char* lisp_format(char* format,
                         slist_elem* next)
{
  int i=0;
  int retsz=0;
  int retpos=0;
  int slen=0;
  int fcount=1;
  char at=0;
  char colon=0;
  char* ret_str=0;
  slist_elem* sle=0;
  lisp_atom* current_arg=0;
  lisp_atom* first_arg=0;

  if(!format)
    return 0;

  /* please make sure this is null-terminated...*/
  slen=strlen(format);
  /* allocate an initial buffer the same 
     length as the format string*/
  ret_str=ALLOC_STR(slen);
  ret_str[slen]='\0';
  retsz=slen;
  retpos=0;

  sle=next;
  first_arg=current_arg=sle?ATOM_CAST(next):0;

  for(;i<slen;++i,at=0,colon=0,fcount=1)
    if(format[i]=='~')
    {
      ++i;
      /* extract the format count */
      if(i<slen&&isdigit(format[i]))
      {
        fcount=atoi(&format[i]);
        while(i<slen&&isdigit(format[++i]));
      }
      /* set any colons or @'s*/
      if(format[i]==':'){colon=1;++i;}
      if(format[i]=='@'){at=1;++i;}

      if(i>=slen)
        return 0;

      /*now determine which format directives are being specified*/
      switch(format[i])
      {
      case '|'://page separators
        for(;fcount>0;--fcount)
          WRITE_BUFFER_C(ret_str,'|',retpos,retsz);
        break;
      case '%': //new lines
        for(;fcount>0;--fcount)
          WRITE_BUFFER_C(ret_str,'\n',retpos,retsz);
        break;
      //case '&'://fresh lines
      //  printf("\r\n?");
      //  break;
      case '~': //tildes
        for(;fcount>0;--fcount)
          WRITE_BUFFER_C(ret_str,'~',retpos,retsz);
        break;
      case '*'://jumps n arguments forward. 
        if(sle)
        {
          sle=sle->_next;
          for(;sle&&fcount>0;--fcount)
            sle=sle->_next;
          if(sle)
            current_arg=ATOM_CAST(sle);
        }
        break;
   //~:*  => jumps 1 argument backward. 
   //~n:* => jumps n arguments backward.
   //~@*  => jumps to the 0th argument. 
   //~n@* => jumps to the nth argument (beginning from 0) 
      case 'C': //character
        if(fcount)
        {
          WRITE_BUFFER_C(ret_str,(char)fcount,retpos,retsz);
        }else printf("C");
        break;
      case 'T'://tabulation
        if(fcount)
          for(;fcount>0;--fcount)
            printf(" ");
        else WRITE_BUFFER_C(ret_str,'\t',retpos,retsz);
        break;
      case 'A':
        //for(;fcount>=0;--fcount)
        if(!current_arg)
          break;
        switch(current_arg->type)
        {
        case LTID:
        case LTSTR:
          WRITE_BUFFER_S(ret_str,(char*)current_arg->data,retpos,retsz)
          break;
        case LTINT:
          EXTEND_BUFFER(ret_str,retsz,12);
          sprintf(&ret_str[retpos],"%d",*(int*)current_arg->data);
          retpos+=12;
          break;
        case LTFLOAT:
          EXTEND_BUFFER(ret_str,retsz,12);
          sprintf(&ret_str[retpos],"%.4f",*(float*)current_arg->data);
          retpos+=12;
          break;
        default:
          break;
        }
        next=next->_next;
        if(next)
          current_arg=ATOM_CAST(next);
        break;
      case 'D'://decimal
        printf("D");
        break;
      case 'X'://hex
        printf("X");
        break;
      case 'O'://octal
        printf("O");
        break;
      case 'B'://binary
        printf("B");
        break;
      case 'R'://spell an integer
        printf("R");
        break;
      case 'P'://plural
        printf("P");
        break;
      case 'F':// floating point
        printf("F");
        break;
      case 'E'://scientific notation
        printf("E");
        break;
      case 'G'://?general
        printf("G");
        break;
      case '$':// monetary
        printf("$");
        break;
      case '<'://justification
        printf("<");
        break;
      case '('://case conversion
        printf("(");
        break;
      case '{'://iteration
        //for(;fcount>=0;--fcount)
        printf("{");
        break;
      case '!'://flush output port
        printf("!");
        break;
      case '?'://next arg is another format string
        printf(lisp_format((char*)ATOM_CAST(next)->data,next->_next));
        break;
      case '_'://space
        for(;fcount>0;--fcount)
          WRITE_BUFFER_C(ret_str,' ',retpos,retsz);
        break;
      default:
        printf("error");
        break;
      }
    }
    else
    {/* print what we see */
      WRITE_BUFFER_C(ret_str,format[i],retpos,retsz);
    }

  return ret_str;
}

static lisp_atom lp_format(slist_elem* next)
{
  lisp_atom ret;
  ret.type=LTNIL;
  ret.data=(void*)LTNIL;
  lisp_atom* first=ATOM_CAST(next);
  if(first->type==LTSTR)
  {
    ret.eval_type=LENORMAL;
    ret.type=LTSTR;
    ret.data=(void*)lisp_format((char*)first->data,next->_next);
  }
  return ret;
}

void load_format()
{
  lisp_put_symbol("format",(void*)new_atom(LENORMAL,LTCFNPTR,(void*)new_lisp_cfn(1,1,CFN_ARGNOCIEL,lp_format)));
}