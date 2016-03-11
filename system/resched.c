/**
 * @file resched.c
 * @provides resched
 *
 * COSC 3250 / COEN 4820 Assignment 4
 * Modified by Emmali Hanson and Noah Baertsch
 */
/* Embedded XINU, Copyright (C) 2008.  All rights reserved. */

#include <xinu.h>

extern void ctxsw(void *, void *);
/**
 * Reschedule processor to next ready process.
 * Upon entry, currpid gives current process id.  Proctab[currpid].pstate
 * gives correct NEXT state for current process if other than PRREADY.
 * @return OK when the process is context switched back
 */
syscall resched(void)
{
	irqmask im;
	pcb *oldproc;               /* pointer to old process entry */
	pcb *newproc;               /* pointer to new process entry */
	pid_typ head, tail, tempIndex;

	oldproc = &proctab[currpid];

	im = disable();

	/* Age the keys */
	if (AGING){
		head = queuehead(readylist);
		tail = queuetail(readylist);
		tempIndex = queuetab[head].next;
		while(tempIndex != tail){
			queuetab[tempIndex].key++;
			tempIndex = queuetab[tempIndex].next;
		}
	}

	/* prioritize the current process */
	if (PRCURR == oldproc->state)
	{
		oldproc->state = PRREADY;
		prioritize(currpid, readylist, oldproc->priority);
	}

	kprintf("Switching from %d ", currpid);

	/* remove first process in ready queue */
	if ((currpid = dequeue(readylist)) <0){
		//temporary fix
		currpid = 0;
	}
	newproc = &proctab[currpid];
	newproc->state = PRCURR;    /* mark it currently running    */
	kprintf("to %d\r\n", currpid);

#if PREEMPT
	preempt = QUANTUM;          /* reset preemption counter     */
#endif

	ctxsw(&oldproc->regs, &newproc->regs);

	/* The OLD process returns here when resumed. */
	restore(im);
	return OK;
}
