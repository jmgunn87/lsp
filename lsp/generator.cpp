#pragma once
#include "stdafx.h"
#include "json_parse.h"
#include "generator.h"
#include "utility.h"
#include "lp3.h"
#include "lisp_symtbl.h"

typedef enum _OBJTYPE
{ 
  OBJDIR=1,OBJFILE=2,OBJUNKNOWN
} OBJTYPE;

typedef enum _OBJINHERIT
{ 
  INH_NONE=1,INH_PARENT=2,INH_SIBLING
} OBJINHERIT;

static int generate_objects(slist* obj);
static int process_file(char* name,
                        char* _template);
static int process_dir_elems(char* name,
                             slist* elems);
static int process_variables(json_value* vars);
static lisp_atom* json_value2atom(json_value* value);

/*****************************************
 * here we process top level system keys *
 * and prepare the generators enviroment *
 *****************************************/
int run_gen_script(const char* working_dir,
                   slist* jlist)
{
  if(working_dir)
    if(set_working_dir(working_dir)==-1)
      return -1;
  return generate_objects(jlist);
}

static int generate_objects(slist* obj)
{
  json_pair* pair=0;
  json_value* value=0;
  slist_elem* sle=0;
  OBJTYPE type=OBJUNKNOWN;
  OBJINHERIT inheritance=INH_NONE;
  char* name=0;
  char* skeleton=0;

  if(!obj)
    return -1;

  sle=obj->_head;
  while(sle)
  {
    pair=(json_pair*)sle->_data;
    value=pair->value;

    if(strcmp(pair->key,"name")==0){
      if(value->value_type!=JS_STRING)
        return -1;
      else name=(char*)value->value;
    }else if(strcmp(pair->key,"type")==0){
      if(value->value_type!=JS_STRING)
        return -1;
      else if(strcmp((char*)value->value,"dir")==0)  type=OBJDIR;
      else if(strcmp((char*)value->value,"file")==0) type=OBJFILE;
      else return -1;/*unknown object type*/
    }else if(strcmp(pair->key,"inherit")==0){
      if(value->value_type!=JS_STRING)
        return -1;
      else if(strcmp((char*)value->value,"none")==0)
        inheritance;
      else if(strcmp((char*)value->value,"parent")==0)
        inheritance=INH_PARENT;
      else if(strcmp((char*)value->value,"sibling")==0)
        inheritance=INH_SIBLING;
      else return -1;
    }else if(strcmp(pair->key,"template")==0){
      if(value->value_type!=JS_STRING)
        return -1;
      /*set the template used for this object*/
      else skeleton=(char*)value->value;
    }else if(strcmp(pair->key,"variables")==0){
      if(value->value_type!=JS_OBJECT)/*install any template variables*/
        return -1;
      else process_variables(value);
    /* deal with any sub-elements(files) by breaking
       and passing control to the following switch  */
    }else if(strcmp(pair->key,"elements")==0){
      if(value->value_type!=JS_ARRAY)
        return -1;
      else break;
    }
    sle=sle->_next;
  }

  /* now handle the processing for the object*/
  if(type==OBJDIR)
  {
    if(!sle)/* there are no sub elements in this directory */
      return process_dir_elems(name,0);
    else
    {
      /* create the directory and its sub elements */
      pair=(json_pair*)sle->_data;
      value=pair->value;
      return process_dir_elems(name,(slist*)value->value);
    }
  }
  else if(type==OBJFILE)
    return process_file(name,skeleton);
  return -1;
}
static int process_file(char* name,
                        char* _template)
{
  return lp3_gen_file((const char*)_template,//input template
                      (const char*)name);//output generated file
}
static int process_dir_elems(char* name,
                             slist* elems)
{
  slist_elem* sle=0;
  json_value* value=0;

  if(!name)
    return -1;
  /*create the directory and set it
  as our current working dir      */
  if(make_folder(name)==-1)
    return -1;
  if(set_working_dir(name)==-1)
    return -1;

  if(elems)
  {
    sle=elems->_head;
    while(sle)
    {
      value=(json_value*)sle->_data;
      if(value->value_type!=JS_OBJECT||
         generate_objects((slist*)value->value)==-1)
        return -1;
      sle=sle->_next;
    }
  }
  /*traverse back up a directory to 
    where we were previously        */
  if(set_working_dir("..\\")==-1)
    return -1;
  return 1;
}

static void install_json_symbol(void* ud,
                                void* elem)
{
  json_pair* pair=(json_pair*)elem;
  if(!pair)
    return;
  lisp_put_symbol(pair->key,(void*)json_value2atom(pair->value));
}

int process_variables(json_value* vars)
{
  slist* pairs=0;
  if(!vars)
    return -1;
  pairs=(slist*)vars->value;
  if(!pairs||pairs->_size<1)
    return -1;
  slist_each(pairs,0,install_json_symbol);
  return 1;
}

lisp_atom* json_value2atom(json_value* value)
{
  slist_elem* jle=0;
  slist* alist=0;  /* casted list */
  slist* nlist=0;  /* new list */
  lisp_atom* atom=0;

  if(!value)
    return 0;

  switch(value->value_type)
  {
  case JS_OBJECT:
    break;
  case JS_ARRAY:
    nlist=new_slist();
    alist=(slist*)value->value;
    jle=alist->_head;
    while(jle)
    {
      slist_pushb(nlist,(void*)json_value2atom((json_value*)jle->_data));
      jle=jle->_next;
    }
    return new_atom(LEQUOTE,LTLIST,(void*)nlist);
  case JS_STRING:
    return new_atom(LENORMAL,LTSTR,(void*)value->value);
  case JS_NUMBER:
    return new_atom(LENORMAL,LTINT,(void*)value->value);
  case JS_TRUE:
    return new_atom(LENORMAL,LTTRUE,(void*)LTTRUE);
  case JS_FALSE:
    return new_atom(LENORMAL,LTNIL,(void*)LTNIL);
  case JS_NULL:
    return new_atom(LENORMAL,LTNIL,(void*)LTNIL);
  default:
    break;
  }
  return atom;
}

