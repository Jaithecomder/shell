#include "headers.h"

extern char curdir[SIZE];
extern char olddir[SIZE];
extern char homedir[SIZE];
extern char absdir[SIZE];
extern char hist[20][SIZE];
extern char un[FNSIZE];
extern int hsize;
extern int err;

void addhist(char * cmd)
{
    hsize = 0;
    char hfile[SIZE];
    strcpy(hfile, "/home/");
    strcat(hfile, un);
    strcat(hfile, "/.shell_history.txt");
    FILE * fp = fopen(hfile, "r");
    if(fp != NULL)
    {
        char h[SIZE];
        while(fgets(h, SIZE, fp) != NULL)
        {
            strcpy(hist[hsize], h);
            hsize++;
        }
        fclose(fp);
    }
    if(hsize == 0)
    {
        strcpy(hist[0], cmd);
        hsize++;
    }
    else if(strcmp(hist[hsize-1], cmd) == 0)
    {
        return;
    }
    else
    {
        if(hsize < 20)
        {
            strcpy(hist[hsize], cmd);
            hsize++;
        }
        else
        {
            for(int i = 1; i<20; i++)
            {
                strcpy(hist[i-1], hist[i]);
            }
            strcpy(hist[19], cmd);
        }
    }
    fp = fopen(hfile, "w");
    for(int i = 0; i<hsize; i++)
    {
        fprintf(fp, "%s", hist[i]);
    }
    fclose(fp);
}

void phist(char * cmd)
{
    char * command = strtok(cmd, " \t\n");
    char * str = strtok(NULL, " \t\n");
    if(str != NULL)
    {
        fprintf(stderr, KRED"history: too many arguments\n"RST);
        err = 1;
        return;
    }
    hsize = 0;
    char hfile[FNSIZE];
    strcpy(hfile, "/home/");
    strcat(hfile, un);
    strcat(hfile, "/.shell_history.txt");
    FILE * fp = fopen(hfile, "r");
    if(fp != NULL)
    {
        char h[SIZE];
        while(fgets(h, SIZE, fp) != NULL)
        {
            strcpy(hist[hsize], h);
            hsize++;
        }
        for(int i = hsize - 10; i<hsize; i++)
        {
            if(i >= 0)
            {
                printf("%s", hist[i]);
            }
        }
    }
    fclose(fp);
    return;
}
