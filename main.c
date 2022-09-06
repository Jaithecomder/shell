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
char un[FNSIZE];
char hn[FNSIZE];
char hist[20][SIZE];
int hsize = 0;
int err = 0;
time_t ti = 0;
pid_t sh_pgid;
struct termios term; 

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
            setpgrp();
            // pid_t p = getpid();
            // setpgid(p, p);
            signal (SIGTTOU, SIG_DFL);
            signal (SIGTTIN, SIG_DFL);
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
            int e = execvp(command, argv);
            if(e == -1)
            {
                printf(KRED"shell: command not found: %s\n"RST, command);
                err = 1;
            }
            exit(0);
        }
        else
        {
            tcsetpgrp(STDIN_FILENO, f);
            int wstat;
            time_t t1, t2;
            t1 = time(NULL);
            waitpid(f, &wstat, 0);
            // if(wstat < 0)
            // {
            //     kill(f, 0);
            // }
            t2 = time(NULL);
            ti = t2 - t1;
            tcsetpgrp(STDIN_FILENO, sh_pgid);
            tcsetattr(STDIN_FILENO, 0, &term);
        }
    }
    return;
}

void bg(char * str)
{
    err = 0;
    char str1[SIZE];
    strcpy(str1, str);
    char * command = strtok(str, " \t\n");
    if(command == NULL)
    {
        return;
    }
    pid_t f = fork();
    if(f == 0)
    {
        setpgrp();
        signal (SIGTTOU, SIG_DFL);
        signal (SIGTTIN, SIG_DFL);
        // pid_t p = getpid();
        // setpgid(p, p);
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
        int e = execvp(command, argv);
        fprintf(stderr, KRED"\nshell"RST);
        err = 1;
        exit(-1);
    }
    else
    {
        setpgid(f, f);
        insertlist(command, f);
        printf("[%d] %d\n", head.pid, f);
    }
}

int main()
{
    head.nxt = NULL;
    head.pid = 0;
    uid_t uid = geteuid();
    struct passwd * pd = getpwuid(uid);
    strcpy(un, pd->pw_name);
    if(errno < 0)
    {
        perror("shell");
    }
    gethostname(hn, FNSIZE);
    if(errno < 0)
    {
        perror("shell");
    }
    getcwd(homedir, FNSIZE);
    if(errno < 0)
    {
        perror("shell");
    }
    strcpy(olddir, homedir);
    strcpy(absdir, homedir);
    
    signal (SIGTTOU, SIG_IGN);
    signal (SIGTTIN, SIG_IGN);
    
    struct sigaction na;
    sigemptyset(&na.sa_mask);
    na.sa_flags = SA_RESTART | SA_SIGINFO;
    na.sa_sigaction = bgend;
    if(sigaction(SIGCHLD, &na, NULL) == -1)
    {
        perror("sig");
    }

    sh_pgid = getpid();
    tcsetpgrp(STDIN_FILENO, sh_pgid);
    tcgetattr(STDIN_FILENO, &term);

    while (1)
    {
        rel(absdir, curdir);
        prompt(un, hn, curdir);
        // TAKE INPUT HERE
        char input[SIZE], tinp[SIZE];
        fgets(input, SIZE, stdin);
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
