#include "headers.h"

extern char curdir[SIZE];
extern char olddir[SIZE];
extern char homedir[SIZE];
extern char absdir[SIZE];
extern int err;

void discover(char * cmd)
{
    int i1 = -1, i2 = -1, len = strlen(cmd), qc = 0;
    for(int i = 0; i<len; i++)
    {
        if(cmd[i] == '\"')
        {
            qc++;
            if(qc == 1)
            {
                i1 = i;
            }
            if(qc == 2)
            {
                i2 = i;
            }
            if(qc == 3)
            {
                fprintf(stderr, KRED"discover: too many search arguments: only one string enclosed in \"\" is allowed\n"RST);
                err = 1;
                return;
            }
        }
    }
    if(i2 == i1+1)
    {
        fprintf(stderr, KRED"discover: empty string is an invalid file/directory name\n"RST);
        err = 1;
        return;
    }
    if(qc == 1)
    {
        fprintf(stderr, KRED"discover: file/directory name string should be enclosed in \"\n"RST);
        err = 1;
        return;
    }
    char dirname[SIZE];
    strcpy(dirname, ".");
    int d = 0, f = 0, dnc = 0;
    if(qc == 0)
    {
        char * command = strtok(cmd, " \t\n");
        char * str = strtok(NULL, " \t\n");
        while(str != NULL)
        {
            if(dnc == 2)
            {
                fprintf(stderr, KRED"discovery: only one source  directory allowed\n"RST);
                err = 1;
                return;
            }
            if(strncmp(str, "-df", 3) == 0 || strncmp(str, "-fd", 3) == 0)
            {
                d = 1;
                f = 1;
            }
            else if(strncmp(str, "-d", 2) == 0)
            {
                d = 1;
            }
            else if(strncmp(str, "-f", 2) == 0)
            {
                f = 1;
            }
            else
            {
                dnc++;
                strcpy(dirname, str);
            }
            str = strtok(NULL, " \t\n");
        }

        if(d == 0 && f == 0)
        {
            d = 1;
            f = 1;
        }

        char apath[SIZE];
        reltoabs(dirname, apath);
        struct stat temp;
        if(lstat(apath, &temp) < 0)
        {
            fprintf(stderr, KRED"discovery: cannot open '%s': "RST, dirname);
            perror("");
            err = 1;
            return;
        }
        if(!S_ISDIR(temp.st_mode))
        {
            fprintf(stderr, KRED"discovery: '%s' not a valid directory\n"RST, dirname);
            err = 1;
            return;
        }
        if(d == 1)
        {
            printf("%s\n", dirname);
        }

        dp(dirname, d, f);
        return;
    }

    if(qc == 2)
    {
        if(cmd[i1+1] == ' ' || cmd[i2-1] == ' ')
        {
            fprintf(stderr, KRED"discover: file/directory name cannot start or end with space\n"RST);
            err = 1;
            return;
        }
        cmd[i1] = '\0';
        cmd[i2] = '\0';
        char search[SIZE];
        strcpy(search, &cmd[i1+1]);
        char * command = strtok(cmd, " \t\n");
        char * str = strtok(NULL, " \t\n");
        while(str != NULL)
        {
            if(dnc == 2)
            {
                fprintf(stderr, KRED"discovery: only one source  directory allowed\n"RST);
                err = 1;
                return;
            }
            if(strncmp(str, "-df", 3) == 0 || strncmp(str, "-fd", 3) == 0)
            {
                d = 1;
                f = 1;
            }
            else if(strncmp(str, "-d", 2) == 0)
            {
                d = 1;
            }
            else if(strncmp(str, "-f", 2) == 0)
            {
                f = 1;
            }
            else
            {
                dnc++;
                strcpy(dirname, str);
            }
            str = strtok(NULL, " \t\n");
        }

        str = strtok(&cmd[i2+1], " \t\n");
        
        while(str != NULL)
        {
            if(dnc == 2)
            {
                fprintf(stderr, KRED"discovery: only one source  directory allowed\n"RST);
                err = 1;
                return;
            }
            if(strncmp(str, "-df", 3) == 0 || strncmp(str, "-fd", 3) == 0)
            {
                d = 1;
                f = 1;
            }
            else if(strncmp(str, "-d", 2) == 0)
            {
                d = 1;
            }
            else if(strncmp(str, "-f", 2) == 0)
            {
                f = 1;
            }
            else
            {
                dnc++;
                strcpy(dirname, str);
            }
            str = strtok(NULL, " \t\n");
        }

        if(d == 0 && f == 0)
        {
            d = 1;
            f = 1;
        }
        
        char apath[SIZE];
        reltoabs(dirname, apath);
        struct stat temp;
        if(lstat(apath, &temp) < 0)
        {
            fprintf(stderr, KRED"discovery: cannot open '%s': "RST, dirname);
            perror("");
            err = 1;
            return;
        }
        if(!S_ISDIR(temp.st_mode))
        {
            fprintf(stderr, KRED"discovery: '%s' not a valid directory\n"RST, dirname);
            err = 1;
            return;
        }
        if(d == 1 && strcmp(getname(dirname), search) == 0)
        {
            printf("%s\n", dirname);
        }

        ds(dirname, d, f, search);
    }
    return;
}

void dp(char * dir, int d, int f)
{
    char dname[SIZE];
    reltoabs(dir, dname);
    struct dirent * e;
    DIR * directory = opendir(dname);
    if(directory == NULL)
    {
        fprintf(stderr, KRED"discover: cannot access '%s': "RST, dir);
        perror("");
        err = 1;
        return;
    }
    e = readdir(directory);
    int bl = 0;
    while(e != NULL)
    {
        if(strcmp(e->d_name, "..") == 0 || strcmp(e->d_name, ".") == 0)
        {
            e = readdir(directory);
            continue;
        }
        char spath[SIZE];
        strcpy(spath, dname);
        strcat(spath, "/");
        strcat(spath, e->d_name);
        struct stat temp;
        lstat(spath, &temp);
        if(S_ISDIR(temp.st_mode))
        {
            char ndir[SIZE];
            strcpy(ndir, dir);
            if(dir[strlen(dir) - 1] == '/')
            {
                if(d == 1)
                {
                    printf("%s%s\n", dir, e->d_name);
                }
                strcat(ndir, e->d_name);
            }
            else
            {
                if(d == 1)
                {
                    printf("%s/%s\n", dir, e->d_name);
                }
                strcat(ndir, "/");
                strcat(ndir, e->d_name);
            }
            dp(ndir, d, f);
        }
        else
        {
            if(dir[strlen(dir) - 1] == '/')
            {
                if(f == 1)
                {
                    printf("%s%s\n", dir, e->d_name);
                }
            }
            else
            {
                if(f == 1)
                {
                    printf("%s/%s\n", dir, e->d_name);
                }
            }
        }

        e = readdir(directory);
    }
    closedir(directory);
    return;
}

void ds(char * dir, int d, int f, char * s)
{
    char dname[SIZE];
    reltoabs(dir, dname);
    struct dirent * e;
    DIR * directory = opendir(dname);
    if(directory == NULL)
    {
        fprintf(stderr, KRED"discover: cannot access '%s': "RST, dir);
        perror("");
        err = 1;
        return;
    }
    e = readdir(directory);
    int bl = 0;
    while(e != NULL)
    {
        if(strcmp(e->d_name, "..") == 0 || strcmp(e->d_name, ".") == 0)
        {
            e = readdir(directory);
            continue;
        }
        char spath[SIZE];
        strcpy(spath, dname);
        strcat(spath, "/");
        strcat(spath, e->d_name);
        struct stat temp;
        lstat(spath, &temp);
        if(S_ISDIR(temp.st_mode))
        {
            char ndir[SIZE];
            strcpy(ndir, dir);
            if(dir[strlen(dir) - 1] == '/')
            {
                if(d == 1 && strcmp(s, e->d_name) == 0)
                {
                    printf("%s%s\n", dir, e->d_name);
                }
                strcat(ndir, e->d_name);
            }
            else
            {
                if(d == 1 && strcmp(s, e->d_name) == 0)
                {
                    printf("%s/%s\n", dir, e->d_name);
                }
                strcat(ndir, "/");
                strcat(ndir, e->d_name);
            }
            ds(ndir, d, f, s);
        }
        else
        {
            if(dir[strlen(dir) - 1] == '/')
            {
                if(f == 1 && strcmp(s, e->d_name) == 0)
                {
                    printf("%s%s\n", dir, e->d_name);
                }
            }
            else
            {
                if(f == 1 && strcmp(s, e->d_name) == 0)
                {
                    printf("%s/%s\n", dir, e->d_name);
                }
            }
        }

        e = readdir(directory);
    }
    closedir(directory);
    return;
}
