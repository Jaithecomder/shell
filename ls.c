#include "headers.h"

extern char curdir[SIZE];
extern char olddir[SIZE];
extern char homedir[SIZE];
extern char absdir[SIZE];
extern int err;

int hcmp(const void * _a, const void * _b)
{
    const char * a = (const char *) _a;
    const char * b = (const char *) _b;
    int x = 0, y = 0;
    while(a[x] == '.' || a[x] == '/')
    {
        x++;
    }
    while(b[y] == '.' || b[y] == '/')
    {
        y++;
    }
    if(strcasecmp(&a[x], &b[y]) != 0)
    {
        return strcasecmp(&a[x], &b[y]);
    }
    else
    {
        return x - y;
    }
}

void ls(char * cmd)
{
    char * command = strtok(cmd, " \t\n");
    char dirs[200][SIZE], files[200][SIZE], npath[SIZE];
    int a = 0, l = 0, d = 0, f = 0, arg = 0;
    char * str = strtok(NULL, " \t\n");
    while(str != NULL)
    {
        if(strncmp(str, "-la", 3) == 0 || strncmp(str, "-al", 3) == 0)
        {
            l = 1;
            a = 1;
        }
        else if(strncmp(str, "-a", 2) == 0)
        {
            a = 1;
        }
        else if(strncmp(str, "-l", 2) == 0)
        {
            l = 1;
        }
        else
        {
            arg++;
            if(d >= 200 || f >= 200)
            {
                fprintf(stderr, KRED"ls: too many arguments: (max limit: 200)\n"RST);
                err = 1;
                return;
            }
            struct stat finfo;
            reltoabs(str, npath);
            if(lstat(npath, &finfo) < 0)
            {
                fprintf(stderr, KRED"ls: cannot access '%s': No such file or directory\n\n"RST, str);
            }
            else if(S_ISDIR(finfo.st_mode))
            {
                strcpy(dirs[d], str);
                d++;
            }
            else if(S_ISREG(finfo.st_mode) || S_ISCHR(finfo.st_mode) || S_ISBLK(finfo.st_mode) || S_ISFIFO(finfo.st_mode) || S_ISLNK(finfo.st_mode) || S_ISSOCK(finfo.st_mode))
            {
                reltoabs(str, files[f]);
                f++;
            }
            else
            {
                if(str[0] == '-')
                {
                    fprintf(stderr, KRED"ls: invalid option -- '%c'\n"RST, str[1]);
                    err = 1;
                    return;
                }
                fprintf(stderr, KRED"ls: cannot access '%s': No such file or directory\n"RST, str);
            }
        }
        str = strtok(NULL, " \t\n");
    }

    if( f == 0 && d == 0)
    {
        if(arg > 0)
        {
            err = 1;
            return;
        }
        char dirname[SIZE];
        getcwd(dirname, SIZE);
        char  flist[SIZE][FNSIZE];
        int ind = 0;
        struct dirent * e;
        DIR * directory = opendir(dirname);
        if(directory == NULL)
        {
            fprintf(stderr, KRED"ls: cannot open '%s': "RST, dirname);
            perror("");
            err = 1;
            return;
        }
        e = readdir(directory);
        int bl = 0;
        while(e != NULL)
        {
            char spath[SIZE];
            getcwd(spath, SIZE);
            strcat(spath, "/");
            strcat(spath, e->d_name);
            struct stat temp;
            stat(spath, &temp);
            if(e->d_name[0] == '.')
            {
                if(a == 1)
                {
                    strcpy(flist[ind], e->d_name);
                    ind++;
                    bl += temp.st_blocks;
                }
            }
            else
            {
                strcpy(flist[ind], e->d_name);
                ind++;
                bl += temp.st_blocks;
            }
            e = readdir(directory);
        }
        closedir(directory);
        qsort(flist, ind, FNSIZE, hcmp);
        if(l == 1)
        {
            printf("total %d\n", bl/2);
        }
        for(int j = 0; j<ind; j++)
        {
            list(flist[j], l, dirname);
        }
        printf("\n");
    }

    qsort(files, f, SIZE, hcmp);
    qsort(dirs, d, SIZE, hcmp);
    
    for(int i = 0; i<f; i++)
    {
        list(files[i], l, "");
    }

    for(int i = 0; i<d; i++)
    {
        if(f != 0 || d > 1)
        {
            printf("%s:\n", dirs[i]);
        }
        char  flist[SIZE][FNSIZE];
        int ind = 0;
        struct dirent * e;
        reltoabs(dirs[i], npath);
        DIR * directory = opendir(npath);
        if(directory == NULL)
        {
            printf(KRED"ls: cannot open '%s': "RST, dirs[i]);
            fflush(stdout);
            perror("");
        }
        e = readdir(directory);
        int bl = 0;
        while(e != NULL)
        {
            char spath[SIZE];
            getcwd(spath, SIZE);
            strcat(spath, "/");
            strcat(spath, e->d_name);
            struct stat temp;
            stat(spath, &temp);
            if(e->d_name[0] == '.')
            {
                if(a == 1)
                {
                    strcpy(flist[ind], e->d_name);
                    ind++;
                    bl += temp.st_blocks;
                }
            }
            else
            {
                strcpy(flist[ind], e->d_name);
                ind++;
                bl += temp.st_blocks;
            }
            e = readdir(directory);
        }
        closedir(directory);
        qsort(flist, ind, FNSIZE, hcmp);
        if(l == 1)
        {
            printf("total %d\n", bl/2);
        }
        for(int j = 0; j<ind; j++)
        {
            list(flist[j], l, npath);
        }
        printf("\n");
    }
}


void list(char * fname, int l, char * dir)
{
    struct stat file;
    char npath[SIZE];
    if(strcmp(dir, "") == 0)
    {
        strcpy(npath, fname);
        lstat(npath, &file);
    }
    else
    {
        strcpy(npath, dir);
        strcat(npath, "/");
        strcat(npath, fname);
        lstat(npath, &file);
    }
    if(l == 0)
    {
        if(S_ISDIR(file.st_mode))
        {
            printf(KBLU"%s\n"RST, fname);
        }
        else if(S_ISLNK(file.st_mode))
        {
            printf(KCYN"%s\n"RST, fname);
        }
        else if((file.st_mode & S_IXUSR) != 0 || (file.st_mode & S_IXGRP) != 0 || (file.st_mode & S_IXOTH) != 0)
        {
            printf(KCYN"%s\n"RST, fname);
        }
        else
        {
            printf("%s\n", fname);
        }
        return;
    }
    else if(l == 1)
    {
        if(S_ISREG(file.st_mode))
        {
            printf("-");
        }
        else if(S_ISBLK(file.st_mode))
        {
            printf("b");
        }
        else if(S_ISCHR(file.st_mode))
        {
            printf("c");
        }
        else if(S_ISDIR(file.st_mode))
        {
            printf("d");
        }
        else if(S_ISLNK(file.st_mode))
        {
            printf("l");
        }
        else if(S_ISFIFO(file.st_mode))
        {
            printf("p");
        }
        else if(S_ISSOCK(file.st_mode))
        {
            printf("SOCK");
        }
        else
        {
            printf("?");
        }
        if((file.st_mode & S_IRUSR) != 0)
        {
            printf("r");
        }
        else
        {
            printf("-");
        }
        if((file.st_mode & S_IWUSR) != 0)
        {
            printf("w");
        }
        else
        {
            printf("-");
        }
        if((file.st_mode & S_IXUSR) != 0)
        {
            printf("x");
        }
        else
        {
            printf("-");
        }
        if((file.st_mode & S_IRGRP) != 0)
        {
            printf("r");
        }
        else
        {
            printf("-");
        }
        if((file.st_mode & S_IWGRP) != 0)
        {
            printf("w");
        }
        else
        {
            printf("-");
        }
        if((file.st_mode & S_IXGRP) != 0)
        {
            printf("x");
        }
        else
        {
            printf("-");
        }
        if((file.st_mode & S_IROTH) != 0)
        {
            printf("r");
        }
        else
        {
            printf("-");
        }
        if((file.st_mode & S_IWOTH) != 0)
        {
            printf("w");
        }
        else
        {
            printf("-");
        }
        if((file.st_mode & S_IXOTH) != 0)
        {
            printf("x");
        }
        else
        {
            printf("-");
        }
        printf(" ");
        printf("%4lu ", file.st_nlink);
        struct passwd * pwd;
        pwd = getpwuid(file.st_uid);
        printf("%s ", pwd->pw_name);
        struct group * grp;
        grp = getgrgid(file.st_gid);
        printf("%s ", grp->gr_name);
        printf("%7ld ", file.st_size);
        char t[FNSIZE];
        strftime(t, FNSIZE, "%h %d %R", localtime(&file.st_mtime));
        printf("%s ", t);
        if(S_ISLNK(file.st_mode))
        {
            char flink[SIZE];
            int bs = readlink(npath, flink, SIZE);
            if(bs == -1)
            {
                printf(KRED"Invalid link: '%s'\n"RST, fname);
            }
            else
            {
                flink[bs] = '\0';
                printf(KCYN"%s"RST"->"KBLU"%s\n"RST, fname, flink);
            }
        }
        else
        {
            if(S_ISDIR(file.st_mode))
            {
                printf(KBLU"%s\n"RST, fname);
            }
            else if((file.st_mode & S_IXUSR) != 0 || (file.st_mode & S_IXGRP) != 0 || (file.st_mode & S_IXOTH) != 0)
            {
                printf(KCYN"%s\n"RST, fname);
            }
            else
            {
                printf("%s\n", fname);
            }

        }

    }
}
