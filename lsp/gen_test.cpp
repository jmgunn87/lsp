#pragma once
#include "stdafx.h"
#include "generator.h"


void test_gen(const char* in_file,
              const char* out_dir)
{
/* get the pathname to the json file*/
  slist* jlist=json_decode(in_file);
  if(!jlist)
    return;
  /*enter the json symbols into lisp symbol table*/
  run_gen_script(out_dir,jlist);
  //process_json(jlist);
  slist_destroy(jlist,free);  
}