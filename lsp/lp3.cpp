#pragma once
#include "stdafx.h"
#include "lp3.h"
#include "lp3_lisp.h"
#include "utility.h"

/* this is a text file copy which detects 
   embedded lisp and executes it */
int lp3_gen_buffer(char* buffer,
                   FILE* fout)
{
  long buffsize=0;
  int cursor=0;

  if(!buffer||!fout)
    return -1;
  buffsize=strlen(buffer);

  for(;cursor<buffsize;)
  {
    switch(buffer[cursor])
    {
    case '\\':
      ++cursor;
      break;
    case '~':/* see if we have any lp3 */
      ++cursor;
      for(;cursor<buffsize&&
        buffer[cursor+1]!='~'&&
        isspace(buffer[cursor+1]);++cursor);
      if(buffer[cursor]=='(')
      {cursor+=lp3_process(&buffer[cursor])+1;continue;}
      break;
    default:
      break;
    }
    if((int)slist_peekf(ignore_stack)==LP3LISTEN)
      fputc(buffer[cursor],fout);
    ++cursor;
  }
  return 1;
}
int lp3_gen_file(const char* lp3_in,
                 const char* lp3_out)
{
  FILE* fin=0;
  FILE* fout=0;
  char* buffer=0;

  fout=fopen(lp3_out,"w+");
  if(!fout)
    return -1;
  buffer=file_load(lp3_in);
  if(!buffer)
  {
    fclose(fout);
    return 1;
  }

  /* set lisp and lp3's stdout */
  lp_stream=lp3_stream=fout;

  /* load lisp and set lisps standard out */
  load_lp3_lisp();

  //load the ignore stack
  ignore_stack=new_slist();
  slist_pushf(ignore_stack,(void*)LP3LISTEN);

  /* now process the file */
  lp3_gen_buffer(buffer,fout);

  slist_destroy(ignore_stack,0);
  fclose(fout);
  free(buffer);
  return 1;
}
