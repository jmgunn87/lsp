#ifndef GENERATOR_H
#define GENERATOR_H

#pragma once
#include "lisp_types.h"
#include "json_parse.h"

int run_gen_script(const char* working_dir,
                   slist* jlist);
void process_json(slist* jlist);
lisp_atom* json_value2atom(json_value* value);

#endif