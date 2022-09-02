#include "headers.h"

extern char curdir[1024];
extern char olddir[1024];
extern char homedir[1024];
extern char absdir[1024];

int hcmp(const void * _a, const void * _b)
{
    const char * a = (const char *) _a;
    const char * b = (const char *) _b;
    int x = 0, y = 0;
    while(a[x] == '.')
    {
        x++;
    }
    while(b[y] == '.')
    {
        y++;
    }
    return strcasecmp(&a[x], &b[y]);
}


int ccmp(const void * _a, const void * _b)
{
    const char * a =  (const char *) _a;
    const char * b =  (const char *) _b;
    return strcasecmp(a, b);
}

void ls(char * cmd)
{
    char * command = strtok(cmd, " \t\n");
    char dirs[200][1024], files[200][1024], npath[1024];
    int a = 0, l = 0, d = 0, f = 0;
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
            if(d >= 200 || f >= 200)
            {
                printf("ls: too many arguments: (max limit: 200)");
                return;
            }
            struct stat finfo;
            reltoabs(str, npath);
            if(lstat(npath, &finfo) < 0)
            {
                perror("ls");
                return;
            }
            if(S_ISDIR(finfo.st_mode))
            {
                strcpy(dirs[d], str);
                d++;
            }
            else if(S_ISREG(finfo.st_mode) || S_ISCHR(finfo.st_mode) || S_ISBLK(finfo.st_mode) || S_ISFIFO(finfo.st_mode) || S_ISLNK(finfo.st_mode) || S_ISSOCK(finfo.st_mode))
            {
                strcpy(files[f], str);
                f++;
            }
            else
            {
                if(str[0] == '-')
                {
                    printf("ls: invalid option -- '%c'\n", str[1]);
                }
                printf("ls: cannot access'%s': No such file or directory\n", str);
                return;
            }
        }
        str = strtok(NULL, " \t\n");
    }

    if( f == 0 && d == 0)
    {
        char dirname[1024];
        getcwd(dirname, 1024);
        char  flist[200][1024];
        int ind = 0;
        struct dirent * e;
        DIR * directory = opendir(dirname);
        if(directory == NULL)
        {
            perror("ls");
        }
        e = readdir(directory);
        int bl = 0;
        while(e != NULL)
        {
            char spath[1024];
            getcwd(spath, 1024);
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
        qsort(flist, ind, 1024, hcmp);
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

    qsort(files, f, 1024, ccmp);
    qsort(dirs, d, 1024, ccmp);
    
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
        char  flist[200][1024];
        int ind = 0;
        struct dirent * e;
        reltoabs(dirs[i], npath);
        DIR * directory = opendir(npath);
        if(directory == NULL)
        {
            perror("ls");
        }
        e = readdir(directory);
        int bl = 0;
        while(e != NULL)
        {
            char spath[1024];
            getcwd(spath, 1024);
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
        qsort(flist, ind, 1024, hcmp);
        if(l == 1)
        {
            printf("total %d\n", bl/2);
        }
        for(int j = 0; j<ind; j++)
        {
            list(flist[j], l, npath);
        }
        printf("\n\n");
    }
}


void list(char * fname, int l, char * dir)
{
    char npath[1024];
    strcpy(npath, dir);
    strcat(npath, "/");
    strcat(npath, fname);
    struct stat file;
    lstat(npath, &file);
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
        else if((file.st_mode & S_IXUSR) != 0)
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
        char t[1024];
        strftime(t, 1024, "%h %d %R", localtime(&file.st_mtime));
        printf("%s ", t);
        if(S_ISLNK(file.st_mode))
        {
            char flink[1024];
            if(readlink(npath, flink, 1024) == -1)
            {
                printf("Invalid link: '%s'\n", fname);
            }
            else
            {
                printf(KCYN"%s"RST"->"KBLU"%s\n"RST, fname, flink);
            }
        }
        else
        {
            if(S_ISDIR(file.st_mode))
            {
                printf(KBLU"%s\n"RST, fname);
            }
            else if((file.st_mode & S_IXUSR) != 0)
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
