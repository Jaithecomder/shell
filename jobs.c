#include "headers.h"

extern struct list head;
extern int err;

int igncmp(char * _a, char * _b)
{
    char a[strlen(_a) + 1];
    char b[strlen(_b) + 1];
    strcpy(a, _a);
    strcpy(b, _b);
    char * str1 = strtok(a, " \t\n");
    char * str2 = strtok(b, " \t\n");
    return strcmp(str1, str2);
}

void jobs(char * cmd)
{
    char * command = strtok(cmd, " \t\n");
    int r = 0, s = 0;
    char * str = strtok(NULL, " \t\n");
    while(str != NULL)
    {
        if(strncmp(str, "-rs", 3) == 0 || strncmp(str, "-sr", 3) == 0)
        {
            s = 1;
            r = 1;
        }
        else if(strncmp(str, "-r", 2) == 0)
        {
            r = 1;
        }
        else if(strncmp(str, "-s", 2) == 0)
        {
            s = 1;
        }
        else if(str[0] == '-')
        {
            fprintf(stderr, KRED "jobs: invalid flag: '%s'\n" RST, str);
            err = 1;
            return;
        }
        else
        {
            fprintf(stderr, KRED "jobs: too many arguments\n" RST);
            err = 1;
            return;
        }
    }
    
    if(r == 0 && s == 0)
    {
        r = 1;
        s = 1;
    }
    struct list * sort = (struct list *) malloc(sizeof(struct list));
    sort->nxt = NULL;
    sort->pid = 0;

    struct list * temp = head.nxt;
    char st;
    while(temp != NULL)
    {
        if((r == 1 && temp->st == 1 && temp->jn != -1) || (s == 1 && temp->st == 0 && temp->jn != -1))
        {
            inssort(temp, sort);
        }
        temp = temp->nxt;
    }

    temp = sort->nxt;
    char tempcmd[SIZE];
    while(temp != NULL)
    {
        if(temp->st == 1)
        {
            strcpy(tempcmd, temp->cmd);
            char * tok = strtok(tempcmd, " \t\n");
            fprintf(stdout, "[%d] Running ", temp->jn);
            while(tok != NULL)
            {
                fprintf(stdout, "%s ", tok);
                tok = strtok(NULL, " \t\n");
            }
            fprintf(stdout, "[%d]\n", temp->pid);
        }
        else if(temp->st == 0)
        {
            strcpy(tempcmd, temp->cmd);
            char * tok = strtok(tempcmd, " \t\n");
            fprintf(stdout, "[%d] Stopped ", temp->jn);
            while(tok != NULL)
            {
                fprintf(stdout, "%s ", tok);
                tok = strtok(NULL, " \t\n");
            }
            fprintf(stdout, "[%d]\n", temp->pid);
        }
        temp = temp->nxt;
    }
    return;
}

void inssort(struct list * temp, struct list * sort)
{
    struct list * new = (struct list *) malloc(sizeof(struct list));
    new->jn = temp->jn;
    strcpy(new->cmd, temp->cmd);
    new->pid = temp->pid;
    new->st = temp->st;
    new->nxt = NULL;
    if(sort->nxt == NULL)
    {
        sort->nxt = new;
        return;
    }
    else if(igncmp(new->cmd, sort->nxt->cmd) < 0)
    {
        new->nxt = sort->nxt;
        sort->nxt = new;
        return;
    }
    else
    {
        struct list * t = sort->nxt;
        while(t->nxt != NULL)
        {
            if(igncmp(new->cmd, t->nxt->cmd) < 0)
            {
                new->nxt = t->nxt;
                t->nxt = new;
                return;
            }
            t = t->nxt;
        }
        t->nxt = new;
        return;
    }
}
