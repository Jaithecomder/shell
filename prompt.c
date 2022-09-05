#include "prompt.h"
#include "headers.h"

extern int err;
extern time_t ti;

void prompt(char * un, char * sn, char * cwd)
{
    if(ti <= 1)
    {
        if(err == 0)
        {
            printf(KBGR "<" RST KBBL "%s" RST "@" KBBL "%s" RST ":" KBGR "%s> " RST, un, sn, cwd);
        }
        else
        {
            printf(KBRD "<" RST KBBL "%s" RST "@" KBBL "%s" RST ":" KBRD "%s> " RST, un, sn, cwd);
        }
    }
    else
    {
        if(err == 0)
        {
            printf(KBGR "<" RST KBBL "%s" RST "@" KBBL "%s" RST ":" KBGR "%s took %lds> " RST, un, sn, cwd, ti);
        }
        else
        {
            printf(KBRD "<" RST KBBL "%s" RST "@" KBBL "%s" RST ":" KBRD "%s took %lds> " RST, un, sn, cwd, ti);
        }
        ti = 0;
    }
}
