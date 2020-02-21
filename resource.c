#include "resource.h"
#include "task.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct reslist reslist= {NULL,NULL};

void add_r(struct res* res)
{
    if(reslist.head==reslist.tail&&reslist.head==NULL)//0
    {
        reslist.head=reslist.tail=res;
        res->next=NULL;

    }
    else if(reslist.head==reslist.tail&&reslist.head!=NULL)//1
    {
        reslist.head=res;
        res->next=reslist.tail;
    }
    else//2
    {
        res->next=reslist.head;
        reslist.head=res;
    }
}

status_type get_resource(resource_type id)
{
    status_type result = STATUS_OK;

    struct res* r=reslist.head;
    while(r!=NULL)
    {
        if(r->id==id)
        {
            break;
        }
        r=r->next;
    }

    if(r->holder==0)//get
    {
        r->holder=current->info.id;

        if(current->stk.head->p < r->ceil_priority)//ceil
        {

            struct rec* rec=malloc(sizeof(struct rec));
            rec->p=r->ceil_priority;
            rec->next=current->stk.head;
            current->stk.head=rec;
        }
        else//not ceil
        {

            struct rec* rec=malloc(sizeof(struct rec));
            rec->p=current->stk.head->p;
            rec->next=current->stk.head;
            current->stk.head=rec;
        }

        re_schedule();

        return STATUS_OK;
    }
    else
    {
        return STATUS_ERROR;
    }

    return result;
}

status_type release_resource(resource_type id)
{
    status_type result = STATUS_OK;

    struct res* r=reslist.head;
    while(r!=NULL)
    {
        if(r->id==id)
        {
            break;
        }
        r=r->next;
    }

    if(r->holder==current->info.id)
    {
        r->holder=0;

        struct rec* t;
        t=current->stk.head->next;


        free(current->stk.head);
        current->stk.head=t;

        re_schedule();

        return STATUS_OK;
    }
    else
    {
        return STATUS_ERROR;
    }

    return result;
}