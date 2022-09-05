#include "headers.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <string.h>
#include <errno.h>

char curdir[SIZE];
char olddir[SIZE];
char homedir[SIZE];
char absdir[SIZE];
char un[1024];
char hn[1024];
char hist[20][SIZE];
int hsize = 0;
int err = 0;
time_t ti = 0;

struct list head;

void fx(char * str)
{
    char str1[SIZE];
    strcpy(str1, str);
    char * command = strtok(str, " \t\n");
    if(command == NULL)
    {
        return;
    }
    err = 0;
    if(strcmp(command, "cd") == 0)
    {
        cd(str1);
    }
    else if(strcmp(command, "echo") == 0)
    {
        echo(str1);
    }
    else if(strcmp(command, "pwd") == 0)
    {
        pwd(str1);
    }
    else if(strcmp(command, "ls") == 0)
    {
        ls(str1);
    }
    else if(strcmp(command, "discover") == 0)
    {
        discover(str1);
    }
    else if(strcmp(command, "history") == 0)
    {
        phist(str1);
    }
    else if(strcmp(command, "pinfo") == 0)
    {
        pinfo(str1);
    }
    else if(strcmp(command, "clear") == 0)
    {
        clear(str1);
    }
    else
    {
        pid_t f = fork();          // since execve switches to new process and ends current one
        if(f == 0)
        {
            char excmd[SIZE];
            strcpy(excmd, str1);
            char *argv[200];
            int argc = 0;
            argv[argc] = strtok(excmd, " \t\n");
            while(argv[argc] != NULL)
            {
                argc++;
                argv[argc] = strtok(NULL, " \t\n");
            }
            int e = execve(command, argv, NULL);
            if(e == -1)
            {
                printf(KRED"shell: command not found: %s\n"RST, command);
                err = 1;
            }
        }
        else
        {
            time_t t1, t2;
            t1 = time(NULL);
            waitpid(f, NULL, 0);
            t2 = time(NULL);
            ti = t2 - t1;
        }
    }
    return;
}

void bg(char * str)
{
    char str1[SIZE];
    strcpy(str1, str);
    char * command = strtok(str, " \t\n");
    if(command == NULL)
    {
        return;
    }
    int f = fork();
    if(f == 0)
    {
        char excmd[SIZE];
        strcpy(excmd, str1);
        char *argv[200];
        int argc = 0;
        argv[argc] = strtok(excmd, " \t\n");
        while(argv[argc] != NULL)
        {
            argc++;
            argv[argc] = strtok(NULL, " \t\n");
        }
        int e = execve(command, argv, NULL);
        if(e == -1)
        {
            printf(KRED"\nshell: command not found: %s"RST, command);
            fflush(stdout);
            err = 1;
            exit(-1);
        }
    }
    else
    {
        insertlist(str, f);
        printf("[%d] %d\n", head.pid, f);
    }
}

int main()
{
    head.nxt = NULL;
    head.pid = 0;
    getlogin_r(un, 1024);
    if(errno < 0)
    {
        perror("");
    }
    gethostname(hn, 1024);
    if(errno < 0)
    {
        perror("");
    }
    getcwd(homedir, 1024);
    if(errno < 0)
    {
        perror("");
    }
    strcpy(olddir, homedir);
    strcpy(absdir, homedir);
    struct sigaction na;
    sigemptyset(&na.sa_mask);
    na.sa_flags = SA_RESTART | SA_SIGINFO;
    na.sa_sigaction = bgend;
    sigaction(SIGCHLD, &na, NULL);
    while (1)
    {
        rel(absdir, curdir);
        prompt(un, hn, curdir);
        // TAKE INPUT HERE
        char input[1024], tinp[1024];
        fgets(input, 1024, stdin);
        int j = 0, len = strlen(input);
        strcpy(tinp, input);
        char * tmptok = strtok(tinp, " \t\n");
        if(tmptok != NULL)
        {
            addhist(input);
        }
        else
        {
            continue;
        }
        for(int i = 0; i<len; i++)
        {
            if(input[i] == ';' || input[i] == '&')
            {
                if(j == i)
                {
                    j = i+1;
                    continue;
                }
                if(j != i)
                {
                    if(input[i] == ';')
                    {
                        input[i] = '\0';
                        fx(&input[j]);
                    }
                    else
                    {
                        input[i] = '\0';
                        bg(&input[j]);
                    }
                    j = i+1;
                    continue;
                }
            }
        }
        if(j < len)
        {
            fx(&input[j]);
        }
    }
}
