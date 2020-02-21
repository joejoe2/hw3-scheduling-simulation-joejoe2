#ifndef TASK_H
#define TASK_H

#include "typedefine.h"
#include "config.h"
#include <ucontext.h>
#include <stdbool.h>

status_type activate_task(task_type id);
status_type terminate_task(void);

struct rec
{
    task_priority_type p;
    struct rec* next;
};

struct stk
{
    struct rec* head;
};

struct tcb
{
    task_const_type info;
    task_state_type state;
    task_priority_type ceil_priority;
    ucontext_t *ctx;
    struct tcb *next,*pre;
    struct stk stk;
    int order;
};

struct queue
{
    struct tcb* head;
    struct tcb* tail;
};

extern struct tcb* current;

extern struct queue readyqueue;
extern struct queue suspend;

extern struct tcb* find_max();

extern void re_schedule();

extern void ctx_switch();

bool is_in(struct queue* queue,task_type id);

void push_t(struct queue* queue,struct tcb* t);
struct tcb* pop_t(struct queue* queue,task_type id);

extern int tatol;

#endif /* TASK_H */