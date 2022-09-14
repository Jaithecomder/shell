#include "headers.h"

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

char input[SIZE];
int len;

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
    else if(strcmp(command, "jobs") == 0)
    {
        jobs(str1);
    }
    else if(strcmp(command, "sig") == 0)
    {
        sig(str1);
    }
    else if(strcmp(command, "fg") == 0)
    {
        fg(str1);
    }
    else if(strcmp(command, "bg") == 0)
    {
        bg(str1);
    }
    else if(strcmp(command, "exit") == 0)
    {
        exit(0);
    }
    else
    {
        pid_t f = fork();          // since execve switches to new process and ends current one
        if(f == 0)
        {
            setpgrp();
            signal(SIGTSTP, SIG_DFL);
            signal (SIGINT, SIG_DFL);
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
                fprintf(stderr, KRED"shell: command not found: %s\n"RST, command);
                err = 1;
            }
            exit(EXIT_FAILURE);
        }
        else
        {
            tcsetpgrp(STDIN_FILENO, f);
            int wstat;
            time_t t1, t2;
            t1 = time(NULL);
            waitpid(f, &wstat, WUNTRACED);
            t2 = time(NULL);
            if(WIFSTOPPED(wstat))
            {
                insertlist(str1, f, 0);
                struct list * temp = getproclist(f);
                printf("\n[%d] %s with pid = %d was stopped.\n", temp->jn, command, f);
            }
            if(WEXITSTATUS(wstat) != EXIT_SUCCESS)
            {
                err = 1;
            }
            ti = t2 - t1;
            tcsetpgrp(STDIN_FILENO, sh_pgid);
        }
    }
    return;
}

void bfx(char * str)
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
            fprintf(stderr, KRED"\nshell: command not found: %s"RST, command);
            err = 1;
        }
        exit(0);
    }
    else
    {
        setpgid(f, f);
        int jn = insertlist(str1, f, 1);
        printf("[%d] %d\n", jn, f);
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
    getcwd(homedir, SIZE);
    if(errno < 0)
    {
        perror("shell");
    }
    strcpy(olddir, homedir);
    strcpy(absdir, homedir);
    
    signal(SIGTSTP, SIG_IGN);
    signal (SIGINT, SIG_IGN);
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

    while (1)
    {
        rel(absdir, curdir);
        prompt(un, hn, curdir);
        char tinp[SIZE], c;
        enableRawMode();
        for(int i=0; i<len; i++)
        {
            input[i] = '\0';
        }
        len = 0;
        while (read(STDIN_FILENO, &c, 1) == 1)
        {
            if(iscntrl(c))
            {
                if(c == 10)
                {
                    printf("\n");
                    fflush(stdout);
                    input[len] = '\n';
                    len++;
                    input[len] = '\0';
                    len++;
                    break;
                }
                else if(c == 127)
                {
                    if(len > 0)
                    {
                        printf("\b \b");
                        fflush(stdout);
                        len--;
                        input[len] = '\0';
                    }
                }
                else if(c == 9)
                {
                    int stat = 0;
                    int l2 = autocomp(input, len, &stat);
                    if(l2 == len && stat != -1)
                    {
                        prompt(un, hn, curdir);
                        for(int i=0; i<len; i++)
                        {
                            printf("%c", input[i]);
                        }
                        fflush(stdout);
                        continue;
                    }
                    len = l2;
                }
                else if(c == 4)
                {
                    exit(0);
                }
            }
            else
            {
                printf("%c", c);
                fflush(stdout);
                input[len] = c;
                len++;
            }
        }
        disableRawMode();
        int j = 0;
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
        int pl = 0;
        for(int i = 0; i<len; i++)
        {
            if(input[i] == '|')
            {
                pl = 1;
            }
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
                        if(pl == 1)
                        {
                            pipeline(&input[j]);
                            pl = 0;
                        }
                        else
                        {
                            ioredir(&input[j]);
                        }
                    }
                    else
                    {
                        input[i] = '\0';
                        if(pl == 1)
                        {
                            fprintf(stderr, KRED "pipeline: cannot pipeline in background\n" RST);
                            err = 1;
                        }
                        else
                        {
                            bfx(&input[j]);
                        }
                    }
                    j = i+1;
                    continue;
                }
            }
        }
        if(j < len && input[j] != '\n')
        {
            if(pl == 1)
            {
                pipeline(&input[j]);
                continue;
            }
            ioredir(&input[j]);
        }
    }
}
