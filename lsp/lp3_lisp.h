#ifndef LP3_LISP_H
#define LP3_LISP_H

#pragma once
#include <stdio.h>
#include "slist.h"

#define LP3IGNORE 666
#define LP3LISTEN 333

extern FILE* lp_stream;    /* ref to lisps stdout */
extern FILE* lp3_stream; /* ref to lp3's stdout */
extern slist* ignore_stack;

void load_lp3_lisp();
int lp3_process(char* str);

#endif