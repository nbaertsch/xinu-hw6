/* COSC3820, COEN4280 Homework 5
 * Modified by Emmali Hanson and Noah Baertsch
 *
 * Implements a priority sorted ready queue.
 *
 *
 */

#include <xinu.h>

qid_typ prioritize(pid_typ pid, qid_typ q, ulong key)
{
	//modify readylist based on priority
	//check priority of new process
	//if priority is higher than head process, make new process head process
	//else go through each item in readylist and compare priority until it is higher
	//then insert the process there

	int head, tail;

	tail = queuetail(q);	//index of head qentry
	head = queuehead(q);	//index of tail qentry

	//DO NOT SORT!!!
	//By adding the new process to the right place in the D-Linked queue,
	//it is guaranteed to be sorted recursively

	pid_typ tempIndex;	//index in the queuetab[]

	//check yourself b4 you wreck yourself
	/* Accept don't because its constantly true.
	 * TODO: Look into this at some point.

	if (isbadqueue(q)){
		kprintf("Bad queue\r\n");
		return SYSERR;
	}
	*/

	if(isbadpid(pid)){
		kprintf("Bad pid\r\n");
		return SYSERR;
	}

	//if your the first to show up
	if(queuetab[head].next == tail){
		//insert between head and tail
		queuetab[head].next = pid;
		queuetab[pid].prev = head;
		queuetab[pid].next = tail;
		queuetab[tail].prev = pid;
		queuetab[pid].key = key;	//set effective priority (should be proctab[pid].priority everytime, no?)
		return q;
	}

	tempIndex = queuetab[head].next;
	while(tempIndex != tail){
		if(key > queuetab[tempIndex].key){
			queuetab[pid].prev = queuetab[tempIndex].prev;
			queuetab[queuetab[pid].prev].next = pid;
			queuetab[pid].next = tempIndex;
			queuetab[tempIndex].prev = pid;
			queuetab[pid].key = key;
			return q;
		}
		tempIndex = queuetab[tempIndex].next;
	}

	//if you make it to the last process w/o finding a home, get in the back of the line
	queuetab[queuetab[tail].prev].next = pid;
	queuetab[pid].prev = queuetab[tail].prev;
	queuetab[pid].next = tail;
	queuetab[tail].prev = pid;
	queuetab[pid].key = key;

	return q;
}


