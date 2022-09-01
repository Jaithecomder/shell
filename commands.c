#include "headers.h"

extern char curdir[1024];
extern char olddir[1024];
extern char homedir[1024];
extern char absdir[1024];

void reltoabs(char * path, char * npath)
{
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
        printf("cd: too many arguments\n");
        return;
    }
    if(strcmp(str1, "~") == 0 || str1 == NULL)
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
            perror("cd: ");
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
    // printf("%s : %s %s", cmd, command, str);
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
        printf("pwd: too many arguments\n");
        return;
    }
    char dir[1024];
    getcwd(dir, 1024);
    printf("%s\n", dir);
    return;
}
