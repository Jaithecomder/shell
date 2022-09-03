#include "headers.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <string.h>
#include <errno.h>

char curdir[1024];
char olddir[1024];
char homedir[1024];
char absdir[1024];
char un[1024];
char hn[1024];
char hist[20][1024];
int hsize = 0;
int err = 0;

void fx(char * str)
{
    char str1[1024];
    strcpy(str1, str);
    char * command = strtok(str, " \t\n");
    if(command == NULL)
    {
        return;
    }
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
    else
    {
        int f = fork();          // since execve switches to new process and ends current one
        if(f == 0)
        {
            char excmd[1024];
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
            waitpid(f, NULL, 0);
        }
    }
    return;
}

void bg(char * str)
{
    char str1[1024];
    strcpy(str1, str);
    char * command = strtok(str, " \t\n");
    if(command == NULL)
    {
        return;
    }
    int f = fork();
    if(f == 0)
    {
        char excmd[1024];
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
}

int main()
{
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
