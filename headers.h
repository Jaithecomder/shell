#include <stdio.h>    
#include "prompt.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

//misc
void reltoabs(char * path, char * npath);

// cd
void cd(char * cmd);
void rel(char * str, char * cur);

//echo
void echo(char * cmd);

//pwd
void pwd(char * cmd);

//ls
void ls(char * cmd);
void list(char * fname, int l);
