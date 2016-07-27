
#include <stdlib.h>
#include <stdio.h>

#include "debug.h"
#include "task.h"
#include "scheduler_algorithm.h"

#define SCHEDULER_NAME "rrp"

const struct scheduler_operations sops;

int verbose_flag = 1;

static int handle;

static struct task *head0 = NULL, *head1 = NULL, *head2 = NULL, *head3 = NULL;

int init_module(void)
{
	handle = register_scheduler(SCHEDULER_NAME, &sops);
	if(handle < 0){
		eprintf("Failed registering scheduler rrp with error %d\n", handle);
		return handle;
	}
	else{
		dprintf("Registered scheduler rrp\n");
	}
	return 0;
}

void cleanup_module(void)
{
	int rv = unregister_scheduler(handle);
	if(rv < 0) eprintf("Error in unregister_scheduler: %d\n", rv);
}

struct task* rrp_enqueue(struct task* r, struct task* t)
{
	struct task *abc = NULL;
	
	if(head3 == NULL && t->task_info->priority == 3)
	{
		head3 = t;
		t->next = head3;
		t->prev = head3;
		abc = head3;
	}
	else if(head2 == NULL && t->task_info->priority == 2)
	{
		head2 = t;
		t->next = head2;
		t->prev = head2;
		abc = head2;
	}
	else if(head1 == NULL && t->task_info->priority == 1)
	{
		head1 = t;
		t->next = head1;
		t->prev = head1;
		abc = head1;
	}
	else if(head0 == NULL && t->task_info->priority == 0)
	{
		head0 = t;
		t->next = head0;
		t->prev = head0;
		abc = head0;
	}
	else
	{
		if(t->task_info->priority == 3)
		{
			head3->prev->next = t;
			t->prev = head3->prev;
			head3->prev = t;
			t->next = head3;
			abc = head3;
			
		}
		else if(t->task_info->priority == 2)
		{
			head2->prev->next = t;
			t->prev = head2->prev;
			head2->prev = t;
			t->next = head2;
			abc = head2;
		
		}
		else if(t->task_info->priority == 1)
		{
			head1->prev->next = t;
			t->prev = head1->prev;
			head1->prev = t;
			t->next = head1;
			abc = head1;
			
		}
		else
		{	head0->prev->next = t;
			t->prev = head0->prev;
			head0->prev = t;
			t->next = head0;
			abc = head0;
		}
	}
	return abc;
}

struct task* rrp_dequeue(struct task* r)
{
	struct task *abc = NULL;
	
	if(r->next == r && r->task_info->priority == 3)
	{ 
		head3 = NULL; 
		abc = head3;
	}
	else if(r->next == r && r->task_info->priority == 2)
	{	
		head2 = NULL;
		abc = head2;
	}
	else if(r->next == r && r->task_info->priority == 1)
	{	
		head1 = NULL;
		abc = head1;
	}
	else if(r->next == r && r->task_info->priority == 0)
	{	
		head0 = NULL;
		abc = head0;
	}
	else
	{
		if(r->task_info->priority == 3)
		{
			r->prev->next = r->next;
			r->next->prev = r->prev;
			head3 = r->next;	
			abc = head3;	
		}
		else if(r->task_info->priority == 2)
		{
			r->prev->next = r->next;
			r->next->prev = r->prev;
			head2 = r->next;
			abc = head2;

		}
		else if(r->task_info->priority == 1)
		{
			r->prev->next = r->next;
			r->next->prev = r->prev;
			head1 = r->next;
			abc = head1;
		}
		else
		{
			r->prev->next = r->next;
			r->next->prev = r->prev;
			head0 = r->next;
			abc = head0;
		}
	}
	return abc;
}

struct task* rrp_periodic_timer(struct task* r)
{

	if(head3)
	{
		head3 = head3->next;
		return head3;
	}
	else if(head2)
	{
		head2 = head2->next;
		return head2;
	}
	else if(head1)
	{
		head1 = head1->next;
		return head1;
	}
	else if(head0)
	{
		head0 = head0->next;
		return head0;
	}
	else
	{
		return NULL;
		
	}
	
}

const struct scheduler_operations sops =
{
	.task_enqueue = rrp_enqueue,
	.task_dequeue = rrp_dequeue,
	.periodic_timer = rrp_periodic_timer,
	.period = 1

};


