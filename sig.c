
#include "headers.h"

extern struct list head;
extern int err;

int strtoint(char * str)
{
    int n = strlen(str);
    int out = 0;
    for(int i = 0; i<n; i++)
    {
        if(str[i] > 47 && str[i] < 58)
        {
            out = out * 10 + str[i] - 48;
        }
        else
        {
            return -1;
        }
    }
    return out;
}

void sig(char * cmd)
{
    char * command = strtok(cmd, " \t\n");
    char * str1 = strtok(NULL, " \t\n");
    if(str1 == NULL)
    {
        fprintf(stderr, KRED "sig: Job number required\n" RST);
        err = 1;
        return;
    }
    int jn = strtoint(str1);
    if(jn == -1)
    {
        fprintf(stderr, KRED "sig: Invalid job number: %s\n" RST, str1);
        err = 1;
        return;
    }
    char * str2 = strtok(NULL, " \t\n");
    if(str2 == NULL)
    {
        fprintf(stderr, KRED "sig: Signal ID required\n" RST);
        err = 1;
        return;
    }
    int sig = strtoint(str2);
    if(sig == -1)
    {
        fprintf(stderr, KRED "sig: Invalid signal ID: %s\n" RST, str2);
        err = 1;
        return;
    }
    char * str3 = strtok(NULL, " \t\n");
    if(str3 != NULL)
    {
        fprintf(stderr, KRED "sig: too many arguments\n" RST);
        err = 1;
        return;
    }
    struct list * temp = head.nxt;
    if(temp == NULL)
    {
        fprintf(stderr, KRED "sig: Job doesn't exist: %d\n" RST, jn);
        err = 1;
        return;
    }
    while(temp != NULL)
    {
        if(temp->jn == jn)
        {
            if(kill(temp->pid, sig) == -1)
            {
                fprintf(stderr, KRED "sig: Signal doesn't exist: %d\n" RST, sig);
                err = 1;
            }
            return;
        }
        temp = temp->nxt;
    }
    fprintf(stderr, KRED "sig: Job doesn't exist: %d\n" RST, jn);
    err = 1;
    return;
}
