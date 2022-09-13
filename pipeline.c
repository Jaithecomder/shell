#include "headers.h"

extern int err;

void pipeline(char * cmd)
{
    int fd[2];
    int len = strlen(cmd);
    int j = 0, pl = 0, oflag = 0;
    int oldin = dup(STDIN_FILENO);
    int oldout = dup(STDOUT_FILENO);
    for(int i = 0; i<len; i++)
    {
        if(cmd[i] == '&')
        {
            fprintf(stderr, KRED "pipeline: background processes not allowed\n" RST);
            err = 1;
            return;
        }
        if(pl > 0 && cmd[i] == '<')
        {
            fprintf(stderr, KRED "pipeline: cannot redirect input in between pipes\n" RST);
            err = 1;
            return;
        }
        if(cmd[i] == '>')
        {
            oflag++;
        }
        if(cmd[i] == '|')
        {
            cmd[i] = '\0';
            pl++;
            if(j == i)
            {
                fprintf(stderr, KRED "pipeline: Empty command not allowed\n" RST);
                err = 1;
                return;
            }
            if(j != i)
            {
                if(oflag > 0)
                {
                    fprintf(stderr, KRED "pipeline: cannot redirect output in between pipes\n" RST);
                    err = 1;
                    return;
                }
                pipe(fd);
                int f = fork();
                if(f == 0)
                {
                    close(fd[0]);
                    dup2(fd[1], STDOUT_FILENO);
                    ioredir(&cmd[j]);
                    dup2(oldout, STDOUT_FILENO);
                    if(err == 1)
                    {
                        exit(1);
                    }
                    exit(0);
                }
                else
                {
                    close(fd[1]);
                    int wstat;
                    waitpid(f, &wstat, 0);
                    if(wstat != 0)
                    {
                        fprintf(stderr, KRED "shell: Invalid command: %s\n" RST, &cmd[j]);
                        err = 1;
                        return;
                    }
                    dup2(fd[0], STDIN_FILENO);
                }
                j = i+1;
            }
        }
    }
    if(j < len && cmd[j] != '\n')
    {
        dup2(oldout, STDOUT_FILENO);
        ioredir(&cmd[j]);
        dup2(oldin, STDIN_FILENO);
    }
    return;
}
