
#include <stdlib.h>
#include <stdio.h>

#include "debug.h"
#include "task.h"
#include "scheduler_algorithm.h"

#define SCHEDULER_NAME "SRTN"

const struct scheduler_operations sops;
struct sched_dat;

int verbose_flag = 1;
int runtime;

static int handle;

static struct task *head = NULL, *tail =NULL;

int init_module(void)
{
	handle = register_scheduler(SCHEDULER_NAME, &sops);
	if(handle < 0)
	{
		eprintf("Failed registering scheduler SRTN with error %d\n", handle);
		return handle;
	}
	else
	{
		dprintf("Registered scheduler SRTN\n");
	}
	return 0;
}

void cleanup_module(void)
{
	int rv = unregister_scheduler(handle);
	if(rv < 0) eprintf("Error in unregister_scheduler: %d\n", rv);
}

struct sch_data
{
	unsigned long last_guess;
	unsigned long last_runtime;
};
typedef struct sch_data sch_t;


struct task* srtn_enqueue(struct task* r, struct task* t)
{
	struct task *abc = NULL;
	sch_t* tempt, *tempr, *tempabc;
	
	if(t->scheduler_data == NULL)
	{
		tempt = (sch_t *)malloc(sizeof(sch_t));
		tempt->last_guess = (2*(4-t->task_info->priority));
		t->scheduler_data = (void *)tempt;
	}
	else if((t->task_info->run_time - tempt->last_guess) <= 0)
	{
		tempt = (sch_t *)t->scheduler_data;
		tempt->last_guess = 0.2*tempt->last_guess + 0.8*(t->task_info->run_time - tempt->last_runtime);
	}
	
	
	
	if(head!=NULL)
	{
		tempr = (sch_t *)r->scheduler_data;
		abc = head;
		tempabc = (sch_t*)abc->scheduler_data;

		while(abc!=NULL)
		{
		
			if(tempabc->last_guess > tempt->last_guess)
			{
				abc=t->next;
			}
			else
			{
				abc->next = t;
				t->next = abc;
			}
		
		}
	}
	else
	{
	
		head = t;
		tail = t;
	
	}
	
	//tempr->last_runtime = r->task_info->run_time;
	tempt->last_runtime = t->task_info->run_time;
	
	return head;
}

struct task* srtn_dequeue(struct task* r)
{
	struct task *abc = head;
	struct task *new = head;
	sch_t* tempr, *tempabc, *tempnew;
	
	if((r!=NULL) && ((sch_t *)r->scheduler_data)!=NULL){
		tempr = (sch_t *)r->scheduler_data;
	}
	
	tempabc = (sch_t *)abc->scheduler_data;
	tempnew = (sch_t *)new->scheduler_data;
	tempr->last_runtime = tempr->last_guess-(tempr->last_runtime - r->task_info->run_time);
	
	if(r->next == r)
	{
		head == NULL;
	}
	
	else
	{
		r->prev->next = r->next;
		r->next->prev = r->prev;
		head = r->next;
	}
	tempr->last_runtime = r->task_info->run_time;
	
	while(abc!=NULL)
	{
	
		if(tempabc->last_guess<tempnew->last_guess)
		{
		 new = abc;
		}
		abc = abc->next;
	}
	return new;
}



struct task* srtn_periodic_timer(struct task* r)
{
	if(head)
		head = head->next;
	return head;
}





const struct scheduler_operations sops =
{
	.task_enqueue = srtn_enqueue,
	.task_dequeue = srtn_dequeue,
	.periodic_timer = srtn_periodic_timer,
	.period = 1
};


