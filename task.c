#include "task.h"
#include "resource.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct queue readyqueue= {NULL, NULL};
struct queue suspend= {NULL, NULL};
struct tcb* current=NULL;

void push_t(struct queue* queue,struct tcb* t)
{
    if(queue->head==queue->tail&&queue->head==NULL)//0
    {
        queue->head=queue->tail=t;
        t->next=t->pre=NULL;
        return;
    }
    else if(queue->head==queue->tail&&queue->head!=NULL)//1
    {
        queue->head=t;
        t->next=queue->tail;
        t->pre=NULL;
        queue->tail->pre=queue->head;
        return;
    }
    else//2
    {
        queue->head->pre=t;
        t->next=queue->head;
        t->pre=NULL;
        queue->head=t;
        return;
    }
}
struct tcb* pop_t(struct queue* queue,task_type id)
{
    if(queue->head==queue->tail&&queue->head==NULL)//0
    {
        return NULL;
    }
    else if(queue->head==queue->tail&&queue->head!=NULL)//1
    {
        struct tcb* t=queue->head;
        if(t->info.id==id)
        {
            queue->head=queue->tail=NULL;
            return t;
        }
        else
        {
            return NULL;
        }
    }
    else if(queue->head!=queue->tail&&queue->head->next==queue->tail)//2
    {
        struct tcb* t=queue->head;
        if(t->info.id==id)
        {
            queue->head=queue->tail;
            queue->head->pre=NULL;
            t->next=NULL;
            return t;
        }
        t=queue->tail;
        if(t->info.id==id)
        {
            queue->tail=queue->head;
            queue->head->next=NULL;
            t->pre=NULL;
            return t;
        }
        return NULL;
    }
    else//3
    {
        struct tcb* t=queue->head;
        //
        if(t->info.id==id)
        {
            queue->head=t->next;
            queue->head->pre=NULL;
            t->next=NULL;
            return t;
        }
        t=queue->tail;
        if(t->info.id==id)
        {
            queue->tail=t->pre;
            queue->tail->next=NULL;
            t->pre=NULL;
            return t;
        }
        //
        t=queue->head->next;
        while(t!=queue->tail)
        {
            if(t->info.id==id)
            {
                t->next->pre=t->pre;
                t->pre->next=t->next;
                t->pre=t->next=NULL;
                return t;
            }
            t=t->next;
        }
        return NULL;
    }
}

status_type activate_task(task_type id)
{
    status_type result = STATUS_OK;

    if(is_in(&suspend,id))
    {
        struct tcb* new=pop_t(&suspend,id);
        new->state=READY;
        new->ceil_priority=new->info.static_priority;
        new->ctx=NULL;
        new->next=new->pre=NULL;
        new->order=total++;


        push_t(&readyqueue,new);

        re_schedule();

        return STATUS_OK;
    }
    else
    {
        return STATUS_ERROR;
    }

    return result;
}

status_type terminate_task(void)
{
    status_type result = STATUS_OK;
    int c=0;
    struct res* r=reslist.head;
    while(r!=NULL)
    {
        if(r->holder==current->info.id)
        {
            c++;
            break;
        }
        r=r->next;
    }

    if(c!=0)
    {
        return STATUS_ERROR;
    }
    else
    {
        current->state=SUSPENDED;
        current->ceil_priority=current->info.static_priority;
        current->ctx=NULL;
        current->next=current->pre;
        current->order=0;

        push_t(&suspend,current);

        re_schedule();

        return STATUS_OK;
    }

    return result;
}