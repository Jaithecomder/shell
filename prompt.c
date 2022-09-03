#include "prompt.h"
#include "headers.h"

extern int err;

void prompt(char * un, char * sn, char * cwd)
{
    if(err == 0)
    {
        printf(KBGR "<" RST KBBL "%s" RST "@" KBBL "%s" RST ":" KBGR "%s> " RST, un, sn, cwd);
    }
    else
    {
        printf(KBRD "<" RST KBBL "%s" RST "@" KBBL "%s" RST ":" KBRD "%s> " RST, un, sn, cwd);
        err = 0;
    }
}
