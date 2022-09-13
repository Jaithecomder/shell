#include "headers.h"

extern int err;

void ioredir(char * cmd)
{
    int len = strlen(cmd);
    int ic = 0, oc = 0, j = 0, ioflag = 0, oldflag;
    int infile, outfile;
    for(int i = 0; i<len; i++)
    {
        oldflag = ioflag;
        if(cmd[i] == '<')
        {
            cmd[i] = '\0';
            ic++;
            ioflag = 1;
        }
        if(ic > 1)
        {
            fprintf(stderr, KRED "io: too many input files\n" RST);
            err = 1;
            return;
        }
        if(cmd[i] == '>')
        {
            cmd[i] = '\0';
            oc++;
            if(cmd[i+1] == '>')
            {
                i++;
                cmd[i] = '\0';
                ioflag = -2;
            }
            else
            {
                ioflag = -1;
            }
        }
        if(oc > 1)
        {
            fprintf(stderr, KRED "io: too many output files\n" RST);
            err = 1;
            return;
        }
        if(cmd[i] == '\n' || cmd[i] == '\0')
        {
            if(j == i)
            {
                fprintf(stderr, KRED "io: Empty file name not allowed\n" RST);
                err = 1;
                return;
            }
            if(j != i)
            {
                if(oldflag == 1)
                {
                    cmd[i] = '\0';
                    infile = oinp(&cmd[j]);
                    if(infile == -1)
                    {
                        fprintf(stderr, KRED "io: Invalid input file\n" RST);
                        err = 1;
                        return;
                    }
                }
                if(oldflag == -1)
                {
                    outfile = oout(&cmd[j]);
                }
                if(oldflag == -2)
                {
                    outfile = oaout(&cmd[j]);
                }
                j = i+1;
                continue;
            }
        }
    }
    int oldin, oldout;
    if(ic > 0)
    {
        oldin = dup(STDIN_FILENO);
        dup2(infile, STDIN_FILENO);
    }
    if(oc > 0)
    {
        oldout = dup(STDOUT_FILENO);
        dup2(outfile, STDOUT_FILENO);
    }
    fx(cmd);
    if(ic > 0)
    {
        dup2(oldin, STDIN_FILENO);
    }
    if(oc > 0)
    {
        dup2(oldout, STDOUT_FILENO);
    }
    return;
}

int oinp(char * str)
{
    char * fname = strtok(str, " \t\n");
    return open(fname, O_RDONLY);
}

int oout(char * str)
{
    char * fname = strtok(str, " \t\n");
    return open(fname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
}

int oaout(char * str)
{
    char * fname = strtok(str, " \t\n");
    return open(fname, O_WRONLY | O_CREAT | O_APPEND, 0644);
}
