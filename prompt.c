#include "prompt.h"
#include "headers.h"

void prompt(char * un, char * sn, char * cwd)
{
    printf("<%s@%s:%s>", un, sn, cwd);
}
