#ifndef LISP_SYMTBL_H
#define LISP_SYMTBL_H

#pragma once
#include "hashtable.h"
#include "lisp_types.h"

/* symbol table functions */
char lisp_put_symbol(const char* name,
                     void* atom);
char lisp_get_symbol(const char* name,
                     void** atom);
char lisp_remove_symbol(const char* name);
char lisp_resolve_id(lisp_atom** atom);

lisp_atom lp_defun(slist_elem* next);
lisp_atom lp_defvar(slist_elem* next);
lisp_atom lp_let(slist_elem* next);

void load_symtbl();

#endif