#pragma once
#include "stdafx.h"
#include "lisp_test_suite.h"
#include "lisp_symtbl.h"
#include "lisp_eval.h"
#include "lisp_math.h"
#include "lisp_cntrl.h"
#include "lisp_cmp.h"
#include "lisp_format.h"
#include "lisp_utils.h"
#include "lisp_io.h"
#include "lisp_access.h"
#include "lisp_conv.h"

/* this is a bog standard c function 
   that prints integer types
   arguments should be pre-evaluated,
   and should count zero or more*/
lisp_atom test_lisp_cfn(slist_elem* args)
{
  lisp_atom* arg=0; /* argument pointer */
  lisp_atom ret;    /* return value */

  /* intialize the return value */
  ret.eval_type=LENORMAL;
  ret.type=LTTRUE;
  ret.data=(void*)LTTRUE;

  /* loop through the arguments */
  while(args)
  {
    arg=(lisp_atom*)args->_data;
    /* make sure the arg is integer type and print it */
    if(arg&&arg->type==LTINT)
      printf("%d,",*(int*)arg->data);
    else printf("?,");
    args=args->_next;
  }
  return ret;
}

lisp_atom exec_lisp(const char* code)
{
  slist* lisp_in=0;
  lisp_atom ret;
  ret.type=LTNIL;

  /* execute the code string */
  lisp_in=lisp_read(code,0);
  if(lisp_in)
    ret=lisp_eval((lisp_atom*)lisp_in->_head->_data,
                  lisp_in->_head->_next);
  
  /* destroy and return */
  slist_destroy(lisp_in,free);
  return ret;
}

void test_lisp()
{
  lisp_atom result;

  load_io();
  load_symtbl();
  load_math_module();
  load_flow();
  load_access();
  load_cmp();

  result=exec_lisp("(defun a(b)(let(u 2)(+ u u)))");
  if(result.type==LTLISPFN)
    printf("DEFUN 'A'\n"); 
  else printf("error\n");
  result=exec_lisp("(a 3)");
  if(result.type==LTINT)
    printf("%d\n",*(int*)result.data);
  else printf("error\n");
  result=exec_lisp("(a 3)");
  if(result.type==LTINT)
    printf("%d\n",*(int*)result.data);
  else printf("error\n");

}



void lisp_repl()
{
  int in_cursor=0;
  char input[256];
  slist* lisp_in=0;
  slist_elem* sle=0;
  FILE* console=0;
  lisp_atom ret;

  load_conv();
  load_io();
  load_format();
  load_utils();
  load_symtbl();
  load_math_module();
  load_flow();
  load_access();
  load_cmp();

   /*open the console get some lists*/
  printf("ReadEvalPrintLoop\r\n");
  console=fopen("CON","r");
  while(1)
  {
    printf(">");
    memset((void*)input,0,256);
    fgets(input,254,console);
    lisp_in=lisp_read((const char*)input,0);
    if(lisp_in)
    {
      sle=lisp_in->_head;
      while(sle)
      {
        ret=lisp_eval((lisp_atom*)sle->_data,sle->_next);
        sle=sle->_next;
        printf("\n");
      }
    }else printf("syntax error: parenthesis unbalanced \n");
  }
}
