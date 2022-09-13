#include "headers.h"

extern struct list head;
extern int err;
extern int ti;
extern pid_t sh_pgid;
extern struct termios term; 

void fg(char * cmd)
{
    char * command = strtok(cmd, " \t\n");
    char * str1 = strtok(NULL, " \t\n");
    if(str1 == NULL)
    {
        fprintf(stderr, KRED "fg: Job number required\n" RST);
        err = 1;
        return;
    }
    int jn = strtoint(str1);
    if(jn == -1)
    {
        fprintf(stderr, KRED "fg: Invalid job number: %s\n" RST, str1);
        err = 1;
        return;
    }
    char * str2 = strtok(NULL, " \t\n");
    if(str2 != NULL)
    {
        fprintf(stderr, KRED "fg: too many arguments\n" RST);
        err = 1;
        return;
    }
    
    struct list * temp = head.nxt;
    if(temp == NULL)
    {
        fprintf(stderr, KRED "fg: Job doesn't exist: %d\n" RST, jn);
        err = 1;
        return;
    }
    while(temp != NULL)
    {
        if(temp->jn == jn)
        {
            tcsetpgrp(STDIN_FILENO, temp->pid);
            kill(temp->pid, SIGCONT);
            int wstat;
            time_t t1, t2;
            t1 = time(NULL);
            deletelist(temp->pid);
            waitpid(temp->pid, &wstat, 0);
            t2 = time(NULL);
            ti = t2 - t1;
            tcsetpgrp(STDIN_FILENO, sh_pgid);
            tcsetattr(STDIN_FILENO, 0, &term);
            return;
        }
        temp = temp->nxt;
    }
    fprintf(stderr, KRED "fg: Job doesn't exist: %d\n" RST, jn);
    err = 1;
    return;
}

void bg(char * cmd)
{
    char * command = strtok(cmd, " \t\n");
    char * str1 = strtok(NULL, " \t\n");
    if(str1 == NULL)
    {
        fprintf(stderr, KRED "bg: Job number required\n" RST);
        err = 1;
        return;
    }
    int jn = strtoint(str1);
    if(jn == -1)
    {
        fprintf(stderr, KRED "bg: Invalid job number: %s\n" RST, str1);
        err = 1;
        return;
    }
    char * str2 = strtok(NULL, " \t\n");
    if(str2 != NULL)
    {
        fprintf(stderr, KRED "bg: too many arguments\n" RST);
        err = 1;
        return;
    }
    
    struct list * temp = head.nxt;
    if(temp == NULL)
    {
        fprintf(stderr, KRED "bg: Job doesn't exist: %d\n" RST, jn);
        err = 1;
        return;
    }
    while(temp != NULL)
    {
        if(temp->jn == jn)
        {
            kill(temp->pid, SIGCONT);
            temp->st = 1;
            return;
        }
        temp = temp->nxt;
    }
    fprintf(stderr, KRED "bg: Job doesn't exist: %d\n" RST, jn);
    err = 1;
    return;
}
