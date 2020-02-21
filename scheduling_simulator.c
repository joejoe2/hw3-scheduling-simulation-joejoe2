#include "resource.h"
#include "task.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ucontext.h>

bool is_in(struct queue* queue,task_type id)
{
    if(queue->head==NULL)
    {
        return false;
    }
    struct tcb* t=queue->head;
    while(t!=NULL)
    {
        if(t->info.id==id)
        {
            return true;
        }

        t=t->next;
    }
    return false;
}

struct tcb* find_max()
{
    struct tcb* t=readyqueue.tail;
    struct tcb* max=t;

    task_priority_type p=0;

    while(t!=NULL)
    {

        t->ceil_priority=t->stk.head->p;
        if(p<t->ceil_priority)
        {
            p=t->ceil_priority;
            max=t;
        }
        else if(p==t->ceil_priority)
        {
            if(t->order<max->order)
            {
                p=t->ceil_priority;
                max=t;
            }
        }
        t=t->pre;

    }

    return max;
}

void re_schedule()
{

    if(current->ctx==NULL)
        current->ctx=malloc(sizeof(ucontext_t));
    getcontext(current->ctx);

    struct tcb* new=find_max();

    current->ceil_priority=current->stk.head->p;

    if(new->ceil_priority<=current->ceil_priority&&current->state!=SUSPENDED)
    {
        return;
    }

    ctx_switch(current,pop_t(&readyqueue,new->info.id));

}

void ctx_switch(struct tcb* from,struct tcb* to)
{
    if(from->state!=SUSPENDED)
    {
        from->state=READY;
        push_t(&readyqueue,from);
    }

    current=to;
    current->state=RUNNING;
    if(to->ctx!=NULL)
    {
        setcontext(to->ctx);
    }
    else
    {
        current->info.entry();
    }

}

int total=0;

int main()
{
    int i;

    for(i=0; i<TASKS_COUNT; ++i)
    {
        entry_point_type entry=task_const[i].entry;/* point to the entry of this task */
        task_type id=task_const[i].id;/* task id */
        task_priority_type static_priority=task_const[i].static_priority;/* the static priority of this task */

        struct tcb* t=malloc(sizeof(struct tcb));
        t->info.entry=entry;
        t->info.id=id;
        t->info.static_priority=static_priority;
        t->state=SUSPENDED;
        t->ceil_priority=static_priority;
        t->next=t->pre=NULL;
        t->ctx=NULL;

        struct rec* rec=malloc(sizeof(struct rec));
        rec->p=static_priority;
        rec->next=t->stk.head;
        t->stk.head=rec;

        push_t(&suspend,t);
    }

    for(i=0; i<AUTO_START_TASKS_COUNT; ++i)
    {
        task_type id=auto_start_tasks_list[i];/* task id */

        struct tcb* t=pop_t(&suspend,id);
        t->state=READY;
        push_t(&readyqueue,t);
    }
    //

    //
    for(i=0; i<RESOURCES_COUNT; ++i)
    {
        resource_type id=resources_id[i];
        task_priority_type ceil_priority=resources_priority[i];

        struct res* r=malloc(sizeof(struct res));
        r->id=id;
        r->holder=0;
        r->ceil_priority=ceil_priority;
        r->next=NULL;
        add_r(r);
    }
    //
    current=NULL;
    ucontext_t context;
    getcontext(&context);
    //

    struct tcb* new=find_max();
    current=pop_t(&readyqueue,new->info.id);
    current->state=RUNNING;
    struct rec* rec=malloc(sizeof(struct rec));
    rec->p=new->info.static_priority;
    new->stk.head=NULL;
    rec->next=new->stk.head;
    new->stk.head=rec;
    new->order=total++;

    new->info.entry();


    while (1);
    return 0;
}

void logq()
{
    printf("id p s ceilp\n");
    printf("ready\n");
    struct tcb* t=readyqueue.head;
    while(t!=NULL)
    {
        printf("%d %d %d %d\n",t->info.id,t->info.static_priority,t->state,t->ceil_priority);
        t=t->next;
    }
    t=suspend.head;
    printf("sus\n");
    while(t!=NULL)
    {
        printf("%d %d %d %d\n",t->info.id,t->info.static_priority,t->state,t->ceil_priority);
        t=t->next;
    }
    printf("current\n");
    t=current;
    printf("%d %d %d %d\n",t->info.id,t->info.static_priority,t->state,t->ceil_priority);
}