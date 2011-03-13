#ifndef UTILITY_H
#define UTILITY_H

#pragma once

char* file_load(const char* filename);
int set_working_dir(const char* dir);
int make_folder(const char* foldername);
int remove_folder(const char* foldername);

#endif