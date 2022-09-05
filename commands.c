#include "headers.h"
#include <errno.h>
#include <stdio.h>
#include <sys/wait.h>

extern char curdir[SIZE];
extern char olddir[SIZE];
extern char homedir[SIZE];
extern char absdir[SIZE];
extern int err;
extern struct list head;

void insertlist(char * cmd, int pid)
{
    head.pid++;
    struct list * new = (struct list *) malloc(sizeof(struct list));
    strcpy(new->cmd, cmd);
    new->pid = pid;
    new->nxt = NULL;
    if(head.nxt == NULL)
    {
        head.nxt = new;
        return;
    }
    struct list * temp;
    temp = head.nxt;
    while(temp->nxt != NULL)
    {
        temp = temp->nxt;
    }
    temp->nxt = new;
    return;
}

void deletelist(int pid)
{
    struct list * temp = head.nxt;
    if(temp == NULL)
    {
        return;
    }
    if(temp->pid == pid)
    {
        head.nxt = head.nxt->nxt;
        free(temp);
        head.pid--;
        return;
    }
    while(temp->nxt != NULL)
    {
        if(temp->nxt->pid == pid)
        {
            struct list * t = temp->nxt;
            temp->nxt = temp->nxt->nxt;
            free(t);
            head.pid--;
            return;
        }
        temp = temp->nxt;
    }
    return;
}


char * getnamelist(int pid)
{
    struct list * temp = head.nxt;
    if(temp == NULL)
    {
        return NULL;
    }
    if(temp->pid == pid)
    {
        head.nxt = head.nxt->nxt;
        free(temp);
        return temp->cmd;
    }
    while(temp->nxt != NULL)
    {
        if(temp->nxt->pid == pid)
        {
            struct list * t = temp->nxt;
            temp->nxt = temp->nxt->nxt;
            free(t);
            return t->cmd;
        }
        temp = temp->nxt;
    }
    return NULL;
}

void clear(char * cmd)
{
    char * command = strtok(cmd, " \t\n");
    char * str = strtok(NULL, " \t\n");
    if(str != NULL)
    {
        printf(KRED"clear: too many arguments\n"RST);
        return;
    }
    printf(CLEAR);
}

void bgend(int sig, siginfo_t * info, void * ucontext)
{
    head.pid--;
    char * temp = getnamelist(info->si_pid);
    if(temp == NULL)
    {
        return;
    }
    if(waitpid(info->si_pid, NULL, WUNTRACED | WCONTINUED) == -1)
    {
        perror("error");
        return;
    }
    char cmd[SIZE];
    strcpy(cmd, temp);
    deletelist(info->si_pid);
    char exstat[30];
    if(info->si_code == CLD_EXITED)
    {
        strcpy(exstat, "has exited normally.");
    }
    else if(info->si_code == CLD_KILLED)
    {
        strcpy(exstat, "was killed.");
    }
    else if(info->si_code == CLD_DUMPED)
    {
        strcpy(exstat, "terminated abnorally.");
    }
    else if(info->si_code == CLD_TRAPPED)
    {
        strcpy(exstat, "has trapped.");
    }
    else if(info->si_code == CLD_STOPPED)
    {
        strcpy(exstat, "was stopped.");
    }
    else if(info->si_code == CLD_CONTINUED)
    {
        strcpy(exstat, "was continued.");
    }
    printf("\n%s with pid = %d %s\n", cmd, info->si_pid, exstat);
    return;
}

void pinfo(char * cmd)
{
    char * command = strtok(cmd, " \t\n");
    char * str1 = strtok(NULL, " \t\n");
    if(str1 == NULL)
    {
        pid_t p = getpid();
        char path[SIZE];
        sprintf(path, "/proc/%d", p);
        struct stat finfo;
        if(lstat(path, &finfo) < 0)
        {
            printf(KRED"pinfo: Invalid pid\n"RST);
            err = 1;
            return;
        }
        char p1[1024], p2[1024], p3[1024];
        strcpy(p1, path);
        strcpy(p2, path);
        strcpy(p3, path);
        strcat(p1, "/stat");
        strcat(p2, "/statm");
        strcat(p3, "/exe");
        int mem;
        FILE * f1 = fopen(p1, "r");
        FILE * f2 = fopen(p2, "r");
        if(f1 == NULL || f2 == NULL)
        {
            printf(KRED"pinfo: error reading information: "RST);
            fflush(stdout);
            perror("");
            err = 1;
            return;
        }
        char status, exec[1024], temp[1024];
        int ppid, pgrp, ss, tt, tgid;
        fscanf(f1, "%d %s %c %d %d %d %d %d", &p, temp, &status, &ppid, &pgrp, &ss, &tt, &tgid);
        fscanf(f2, "%d", &mem);
        printf("pid : %d\n", p);
        printf("process status : %c", status);
        if(pgrp == tgid)
        {
            printf("+");
        }
        printf("\nmemory : %d\n", mem);
        int bs = readlink(p3, exec, 1024);
        if(bs == -1)
        {
            printf(KRED"pinfo: cannot access executable path:"RST);
            fflush(stdout);
            perror("");
        }
        else
        {
            exec[bs] = '\0';
            char r[1024];
            rel(exec, r);
            printf("executable path : %s\n", r);
        }
        fclose(f1);
        fclose(f2);
    }
    else
    {
        char * str2 = strtok(NULL, " \t\n");
        if(str2 != NULL)
        {
            printf(KRED"pinfo: too many arguments\n"RST);
            err = 1;
            return;
        }
        else
        {
            pid_t p;
            int mem;
            char p1[1024], p2[1024], p3[1024];
            strcpy(p1, "/proc/");
            strcat(p1, str1);
            struct stat finfo;
            if(lstat(p1, &finfo) < 0)
            {
                printf(KRED"pinfo: Invalid pid\n"RST);
                err = 1;
                return;
            }
            strcpy(p2, p1);
            strcpy(p3, p1);
            strcat(p1, "/stat");
            strcat(p2, "/statm");
            strcat(p3, "/exe");
            FILE * f1 = fopen(p1, "r");
            FILE * f2 = fopen(p2, "r");
            if(f1 == NULL || f2 == NULL)
            {
                printf(KRED"pinfo: error reading information: "RST);
                fflush(stdout);
                perror("");
                err = 1;
                return;
            }
            char status, exec[1024], temp[1024];
            int ppid, pgrp, ss, tt, tgid;
            fscanf(f1, "%d %s %c %d %d %d %d %d", &p, temp, &status, &ppid, &pgrp, &ss, &tt, &tgid);
            fscanf(f2, "%d", &mem);
            printf("pid : %d\n %d %d\n", p, pgrp, tgid);
            printf("process status : %c", status);
            if(pgrp == tgid)
            {
                printf("+");
            }
            printf("\nmemory : %d\n", mem);
            int bs = readlink(p3, exec, 1024);
            if(bs == -1)
            {
                printf(KRED"pinfo: cannot access executable path:"RST);
                fflush(stdout);
                perror("");
            }
            else
            {
                exec[bs] = '\0';
                char r[1024];
                rel(exec, r);
                printf("executable path : %s\n", r);
            }
            fclose(f1);
            fclose(f2);
        }
    }
}

char * getname(char * f)
{
    int n = strlen(f);
    for(int i = n-1; i>=0; i++)
    {
        if(f[i] == '/')
        {
            return &f[i+1];
        }
    }
    return f;
}

void reltoabs(char * path, char * npath)
{
    if(strcmp(path, "/") == 0)
    {
        strcpy(npath, path);
        return;
    }
    if(path[strlen(path)-1] == '/')
    {
        path[strlen(path)-1] = '\0';
    }
    if(path[0] == '~')
    {
        strcpy(npath, homedir);
        strcat(npath, &path[1]);
        return;
    }
    strcpy(npath, path);
    return;
}

void rel(char * str, char * cur)
{
    if(strncmp(str, homedir, strlen(homedir)) == 0)
    {
        strcpy(cur, "~");
        strcat(cur, &str[strlen(homedir)]);
        return;
    }
    else
    {
        strcpy(cur, str);
        return;
    }
}

void cd(char * cmd)
{
    char * command = strtok(cmd, " \t\n");
    char * str1 = strtok(NULL, " \t\n");
    char * str2 = strtok(NULL, " \t\n");
    if(str2 != NULL)
    {
        printf(KRED"cd: too many arguments\n"RST);
        err = 1;
        return;
    }
    if(str1 == NULL || strcmp(str1, "~") == 0)
    {
        getcwd(olddir, 1024);
        chdir(homedir);
        strcpy(absdir, homedir);
        return;
    }
    else if(strcmp(str1, "-") == 0)
    {
        chdir(olddir);
        strcpy(olddir, absdir);
        getcwd(absdir, 1024);
        printf("%s\n", absdir);
        return;
    }
    else if(str1[0] == '~' && str1[1] == '/')
    {
        char tempold[1024], path[1024];
        getcwd(tempold, 1024);
        strcpy(path, homedir);
        strcat(path, &str1[1]);
        if(chdir(path) < 0)
        {
            perror("cd: ");
            return;
        }
        strcpy(olddir, tempold);
        getcwd(absdir, 1024);
        return;
    }
    else
    {
        char tempold[1024];
        getcwd(tempold, 1024);
        if(chdir(str1) < 0)
        {
            fprintf(stderr, KRED);
            perror("cd");
            fprintf(stderr, RST);
            err = 1;
            return;
        }
        strcpy(olddir, tempold);
        getcwd(absdir, 1024);
        return;
    }
}

void echo(char * cmd)
{
    char * command = strtok(cmd, " \t\n");
    char * str = strtok(NULL, " \t\n");
    while(str != NULL)
    {
        printf("%s ", str);
        str = strtok(NULL, " \t\n");
    }
    printf("\n");
    return;
}

void pwd(char * cmd)
{
    char * command = strtok(cmd, " \t\n");
    char * str = strtok(NULL, " \t\n");
    if(str != NULL)
    {
        printf(KRED"pwd: too many arguments\n"RST);
        err = 1;
        return;
    }
    char dir[1024];
    getcwd(dir, 1024);
    printf("%s\n", dir);
    return;
}
