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
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <termios.h>

#define RST  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define KBRD  "\x1B[91m"
#define KBGR  "\x1B[92m"
#define KBBL  "\x1B[94m"

#define CLEAR "\x1b[2J\x1b[H"

#define SIZE 4096
#define FNSIZE 256

struct list
{
    int jn;
    char cmd[SIZE];
    int st;
    int pid;
    struct list * nxt;
};

// run commands (main)
void fx(char * cmd);
void bfx(char * cmd);

// misc(commands)
void reltoabs(char * path, char * npath);
char * getname(char * f);
void bgend(int sig, siginfo_t * info, void * ucontext);
char getstatus(pid_t pid);

// bglist
int insertlist(char * cmd, int pid);
void deletelist(int pid);
struct list * getproclist(int pid);

// cd (commands)
void cd(char * cmd);
void rel(char * str, char * cur);

// echo (commands)
void echo(char * cmd);

// pwd (commands)
void pwd(char * cmd);

// clear (commands)
void clear(char * cmd);

// ls
void ls(char * cmd);
void list(char * fname, int l, char * dir);

// discover
void discover(char * cmd);
void dp(char * dir, int d, int f);
void ds(char * dir, int d, int f, char * s);

// history
void addhist(char * cmd);
void phist(char * cmd);

// pinfo (commands)
void pinfo(char * cmd);

// udefcom
    // jobs
void jobs(char * cmd);
void inssort(struct list * temp, struct list * sort);

    // sig
void sig(char * cmd);
int strtoint(char * str);

    // fg
void fg(char * cmd);

    // bg
void bg(char * cmd);

// ioredir
void ioredir(char * cmd);
int oinp(char * str);
int oout(char * str);
int oaout(char * str);

// pipeline
void pipeline(char * cmd);
