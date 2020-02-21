#ifndef RESOURCE_H
#define RESOURCE_H

#include "typedefine.h"
#include "config.h"
#include <ucontext.h>

status_type get_resource(resource_type id);
status_type release_resource(resource_type id);

struct res
{
    resource_type id;
    task_type holder;
    task_priority_type ceil_priority;
    struct res* next;
};

struct reslist
{
    struct res* head;
    struct res* tail;
};

void add_r(struct res* res);

extern struct reslist reslist;

extern struct tcb* find_max();

extern void re_schedule();

extern void ctx_switch();

extern int total;

#endif /* RESOURCE_H */