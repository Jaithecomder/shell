#include "headers.h"

extern struct list head;

int insertlist(char * cmd, int pid, int st)
{
    struct list * new = (struct list *) malloc(sizeof(struct list));
    strcpy(new->cmd, cmd);
    new->pid = pid;
    new->st = st;
    new->nxt = NULL;
    if(head.nxt == NULL)
    {
        new->jn = 1;
        head.nxt = new;
        return 1;
    }
    if(head.nxt->jn == -1)
    {
        head.nxt->jn = 1;
        strcpy(head.nxt->cmd, new->cmd);
        head.nxt->pid = new->pid;
        head.nxt->st = new->st;
        free(new);
        return head.nxt->jn;
    }
    struct list * temp;
    temp = head.nxt;
    while(temp->nxt != NULL && temp->nxt->jn != -1)
    {
        temp = temp->nxt;
    }
    new->jn = temp->jn + 1;
    if(temp->nxt != NULL)
    {
        temp->nxt->jn = new->jn;
        strcpy(temp->nxt->cmd, new->cmd);
        temp->nxt->pid = new->pid;
        temp->nxt->st = new->st;
        free(new);
        return temp->nxt->jn;
    }
    temp->nxt = new;
    return new->jn;
}

void deletelist(int pid)
{
    struct list * temp = head.nxt;
    if(temp == NULL)
    {
        return;
    }
    while(temp != NULL)
    {
        if(temp->pid == pid)
        {
            temp->jn = -1;
            return;
        }
        temp = temp->nxt;
    }
    return;
}

struct list * getproclist(int pid)
{
    struct list * temp = head.nxt;
    if(temp == NULL)
    {
        return NULL;
    }
    while(temp != NULL)
    {
        if(temp->pid == pid)
        {
            return temp;
        }
        temp = temp->nxt;
    }
    return NULL;
}
