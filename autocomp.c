#include "headers.h"

int seppath(char * inp, char * path, char * fname)
{
    int n = strlen(inp);
    for(int i = n-1; i>=0; i--)
    {
        if(inp[i] == '/')
        {
            if(i == 0)
            {
                strcpy(path, "/");
            }
            else
            {
                inp[i] = '\0';
                reltoabs(inp, path);
            }
            strcpy(fname, &inp[i+1]);
            return 0;
        }
    }
    strcpy(fname, inp);
    return -1;
}

int autocomp(char * inp, int n, int * stat)
{
    if(inp[n-1] == ' ')
    {
        *stat = -1;
        return n;
    }
    char tinp[SIZE], finp[SIZE], saven[FNSIZE];
    strncpy(tinp, inp, n);
    tinp[n] = '\0';
    char * str = strtok(tinp, " \t\n");
    while(str != NULL)
    {
        strcpy(finp, str);
        str = strtok(NULL, " \t\n");
    }
    char path[SIZE], fname[FNSIZE];
    int  p = seppath(finp, path, fname);
    if(fname[0] == '\0')
    {
        *stat = -1;
        return n;
    }
    if(p == -1)
    {
        getcwd(path, SIZE);
    }
    struct dirent * au;
    DIR * directory = opendir(path);
    if(directory == NULL)
    {
        return n;
    }
    au = readdir(directory);
    int mcount = 0;
    int flength = strlen(fname);
    while(au != NULL)
    {
        if(strncmp(fname, au->d_name, flength) == 0)
        {
            char spath[SIZE];
            strcpy(spath, path);
            strcat(spath, "/");
            strcat(spath, au->d_name);
            struct stat temp;
            lstat(spath, &temp);
            mcount++;
            if(mcount > 1)
            {
                if(mcount == 2)
                {
                    printf("\n");
                }
                printf("%s\n", saven);
            }
            strcpy(saven, au->d_name);
            if(S_ISDIR(temp.st_mode))
            {
                strcat(saven, "/");
            }
        }
        au = readdir(directory);
    }
    if(mcount == 1)
    {
        inp[n] = '\0';
        strcat(inp, &saven[flength]);
        printf("%s", &saven[flength]);
        fflush(stdout);
        *stat = -1;
        return n + strlen(saven) - flength;
    }
    if(mcount > 1)
    {
        printf("%s\n", saven);
        *stat = 0;
    }
    if(mcount == 0)
    {
        *stat = -1;
    }
    return n;
}
