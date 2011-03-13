#ifndef LISP_TYPES_H
#define LISP_TYPES_H

#pragma once
#include "slist.h"

void load_types();

#define CFN_ARGNOCIEL -1
#define ATOM_CAST(sle)( ((lisp_atom*)sle->_data) )
#define LPRETURN(ratom,etype,ltype,adata)\
{\
  ratom.eval_type=etype;\
  ratom.type=ltype;\
  ratom.data=(void*)adata;\
  return ratom;\
}
#define LPRETURN_T(ret) LPRETURN(ret,LENORMAL,LTTRUE,LTTRUE)
#define LPRETURN_NIL(ret) LPRETURN(ret,LENORMAL,LTNIL,LTNIL)

typedef enum _LISP_TYPE
{
  LTNIL=0,LTTRUE=1,LTID=8,LTINT=9,LTFLOAT=10,
  LTSTR=11,LTLIST=12,LTLISPFN=14,LTLISPMACRO=15,
  LTARRAY=16,LTCFNPTR=17
} LISP_TYPE;

typedef enum _LEVAL_TYPE
{
  LENORMAL,LEQUOTE,LEBQUOTE,
  LECOMMA,LECOMMA_AT,LEAT
} LEVAL_TYPE;

typedef struct _lisp_atom
{
  LEVAL_TYPE eval_type;/* indicates wether or not this atom should be evaluated  */
  LISP_TYPE type;
  void* data;
} lisp_atom; 

lisp_atom* new_atom(LEVAL_TYPE et,
                    LISP_TYPE type,
                    void* data);
lisp_atom* atom_copy(lisp_atom* atom);
char atom_truth(lisp_atom* atom);
void atom_destroy(void* patom);
int atom_length(lisp_atom* atom);

typedef lisp_atom(*cfnptr)(slist_elem*);

typedef struct _lisp_cfn
{
  char eval_args;//0 = dont eval, 1+ = eval
  int argc_floor;//0 to *
  int argc_ciel;//0 to ARGNOCIEL
  cfnptr its_fnptr;
} lisp_cfn;

lisp_cfn* new_lisp_cfn(char eval,
                       int floor,
                       int ciel,
                       cfnptr fnptr);


typedef struct _lisp_array
{
  LISP_TYPE type;
  unsigned int length;
  void* data;
} lisp_array;

lisp_array* new_lisp_array(LISP_TYPE type,
                           int length,
                           void* data);

int lisp_array_elem_size(lisp_array* la);

#define ALLOC_BOOL()( (char*)malloc(sizeof(char)) )
#define ALLOC_INT()( (int*)malloc(sizeof(int)) )
#define ALLOC_FLOAT()( (float*)malloc(sizeof(float)) )
#define ALLOC_STR(len)( (char*)malloc(sizeof(char)*len+1) )
#define ALLOC_ARRAY(type,count)( (type*)malloc(sizeof(type)*count) )
#define NEW_ATOM_ID(name)( new_atom(LTID,(void*)name) )
#define NEW_ATOM_STR(str)( new_atom(LTSTR,(void*)str) )
#define NEW_ATOM_INT(val)( new_atom(LTINT,(void*)val) )
#define NEW_ATOM_FLOAT(val)( new_atom(LTFLOAT,(void*)val) )
#define NEW_ATOM_LIST(list)( new_atom(LTINT,(void*)list) )
#define NEW_ATOM_LISTFN(list)( new_atom(LTLISPFN,(void*)list) )
#define NEW_ATOM_LISTMACRO(list)( new_atom(LTLISPMACRO,(void*)list) )
#define NEW_ATOM_FNPTR(fnptr)( new_atom(LTLISPMACRO,(void*)fnptr) )

extern slist* _activation_stack;
char resolve_id(lisp_atom** atom);


#endif