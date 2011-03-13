#pragma once
#include "stdafx.h"
#include "utility.h"
#include <stdio.h>

#ifdef LINUX
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#elif WIN32
#include <direct.h>
#endif

int set_working_dir(const char* dir)
{
#ifdef LINUX
  return chdir(dir)?1:-1;
#elif WIN32
  return _chdir(dir)==0?1:-1;
#endif  
}
int remove_folder(const char* foldername)
{
#ifdef LINUX
  return rmdir(foldername)?1:-1;
#elif WIN32
  return _rmdir(foldername)==0?1:-1;
#endif  
}
int make_folder(const char* foldername)
{
#ifdef LINUX
  return mkdir(foldername,S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH)?1:-1;
#elif WIN32
  return _mkdir(foldername)==0?1:-1;
#endif
}

char* file_load(const char* filename)
{
  FILE* sk_file=0;
  char* sk_buffer=0;
  long sk_size=0;

  if(!filename)
    return 0;

  /*************************
   * try and open the file *
   *************************/
  sk_file=fopen(filename,"r");
  if(!sk_file)
    return 0;

  /******************************************************
   * obtain the file size and allocate for its contents *
   ******************************************************/
  fseek(sk_file,0,SEEK_END);
  sk_size=ftell(sk_file);
  rewind(sk_file);
  sk_buffer=(char*)malloc(sizeof(char)*sk_size);
  
  /*****************************
   * read in the  file         *
   *****************************/
  sk_size=fread(sk_buffer,1,sk_size,sk_file);
  if(!sk_size)
    return 0;

  /******************************************
   * make sure we null terminate the buffer *
   * and close the file                     *
   ******************************************/
  sk_buffer[sk_size]='\0';
  fclose(sk_file);
  return sk_buffer;
}
