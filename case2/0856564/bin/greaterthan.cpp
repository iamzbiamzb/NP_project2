#include <stdio.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <cstring>

FILE *fd;
using namespace std;

int main(int argc, char *argv[])
{
  char c;

  if(argc == 1)
     fd = stdin;
  else if(argc == 2)
     fd = fopen(argv[1], "r");
  else
  {
     fprintf(stderr,"Usage");
     exit(1);
  }
  
  string str="";
  while((c = fgetc(fd))!=EOF)
  {
    str+=c;
    if (c == '\n'){
    if (c == -1)
      break;
      fprintf(stdout, "%s",str.c_str());
      str="";
    }
  } 
  
  fclose(fd);
  return 0;

}


 
