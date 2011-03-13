#ifndef LISP_READER_H
#define LISP_READER_H

#pragma once
#include "lisp_types.h"

slist* lisp_read(const char* str,
                 int* pos);

#endif