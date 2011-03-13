#ifndef LISP_ARRAY_H
#define LISP_ARRAY_H

#pragma once
#include "lisp_types.h"

void array_type_depth(const lisp_array* larray,
                      LISP_TYPE* type,
                      int* depth);
int array_depth_step(const lisp_array* larray,
                     int depth,
                     int width);
void* array_at(lisp_array* la,
               int index);

#endif