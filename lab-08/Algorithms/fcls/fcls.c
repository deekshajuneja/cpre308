
#include <stdlib.h>
#include <stdio.h>

#include "debug.h"
#include "task.h"
#include "scheduler_algorithm.h"

#define SCHEDULER_NAME "FCLS"

const struct scheduler_operations sops;

int verbose_flag = 1;

static int handle;

static struct task *head = NULL, *tail = NULL;

int init_module(void)
{
	handle = register_scheduler(SCHEDULER_NAME, &sops);
	if(handle < 0){
		eprintf("Failed registering scheduler FCLS with error %d\n", handle);
		return handle;
	}
	else{
		dprintf("Registered scheduler FCLS\n");
	}
	return 0;
}

void cleanup_module(void)
{
	int rv = unregister_scheduler(handle);
	if(rv < 0) eprintf("Error in unregister_scheduler: %d\n", rv);
}

struct task* fcls_enqueue(struct task* r, struct task* t)
{
	t->next = NULL;
	if(tail)
	{
		t->prev = tail;
		tail->next = t;
		tail = t;
	}
	else
	{
		head = t;
		tail = t;
	}
	return tail;
}

struct task* fcls_dequeue(struct task* r)
{
	
	if(!tail)
		{head = NULL;
		tail = NULL;
		}
	else
	{tail = tail->prev;
	
	if(!tail)
		head = NULL;
	}
	return tail;
}

const struct scheduler_operations sops =
{
	.task_enqueue = fcls_enqueue,
	.task_dequeue = fcls_dequeue
};


