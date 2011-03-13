// lsp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "lisp_eval.h"
#include "lisp_test_suite.h"
#include "gen_test.h"

int main(int argc,
         char* argv[])
{
  //test_lisp();
  //lisp_repl();

  switch(argc)
  {
  case 0:
  case 1:
    return -1;
  case 2:
    test_gen(argv[1],0);
    return 1;
  case 3:
    test_gen(argv[1],argv[2]);
    return 1;
  default:
    test_gen(argv[1],argv[2]);
    return 1;
  }
  return -1;
}

