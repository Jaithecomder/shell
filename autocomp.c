#include "headers.h"

int autocomp(char * inp, int n, int * stat)
{
    if(inp[n-1] == ' ' || inp[0] == '\0')
    {
        *stat = -1;
        return n;
    }
    char tinp[SIZE], fname[SIZE], saven[FNSIZE];
    strncpy(tinp, inp, n);
    char * str = strtok(tinp, " \t\n");
    while(str != NULL)
    {
        strcpy(fname, str);
        str = strtok(NULL, " \t\n");
    }
    char cdir[SIZE];
    getcwd(cdir, SIZE);
    struct dirent * au;
    DIR * directory = opendir(cdir);
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
            strcpy(spath, cdir);
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
