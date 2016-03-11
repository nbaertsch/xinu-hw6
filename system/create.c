/**
 * @file create.c
 * @provides create, newpid, userret
 *
 * COSC 3250 / COEN 4820 Assignment 4
 */
/* Embedded XINU, Copyright (C) 2008.  All rights reserved. */

#include <xinu.h>

static pid_typ newpid(void);
void userret(void);
void *getstk(ulong);

/**
 * Create a new process to start running a function.
 * @param funcaddr address of function that will begin in new process
 * @param ssize    stack size in bytes
 * @param name     name of the process, used for debugging
 * @param nargs    number of arguments that follow
 * @return the new process id
 */
syscall create(void *funcaddr, ulong ssize, ulong priority, char *name, ulong nargs, ...)
{
    ulong *saddr;               /* stack address                */
    ulong pid;                  /* stores new process id        */
    pcb *ppcb;                  /* pointer to proc control blk  */
    ulong i;
    va_list ap;                 /* points to list of var args   */
    ulong pads = 0;             /* padding entries in record.   */
    void INITRET(void);

    if (ssize < MINSTK)
        ssize = MINSTK;
    ssize = (ulong)(ssize + 3) & 0xFFFFFFFC;
    /* round up to even boundary    */
    saddr = (ulong *)getstk(ssize);     /* allocate new stack and pid   */
    pid = newpid();
    /* a little error checking      */
    if ((((ulong *)SYSERR) == saddr) || (SYSERR == pid))
    {
        return SYSERR;
    }

    numproc++;
    ppcb = &proctab[pid];
    /* setup PCB entry for new proc */
    ppcb->state = PRSUSP;	// initialize state PRSUSP
    ppcb->stkbase = saddr;	// initialize address of stack
    ppcb->stklen = ssize;	// initialize stack length
    ppcb->priority = priority;	//initialize process priority
    strlcpy(ppcb->name, name, PNMLEN);

    /* Initialize stack with accounting block. */
    *saddr = STACKMAGIC;
    *--saddr = pid;
    *--saddr = ppcb->stklen;
    *--saddr = (ulong)ppcb->stkbase;

    /* Handle variable number of arguments passed to starting function   */
    if (nargs)
    {
        pads = ((nargs - 1) / 4) * 4;
    }
    /* If more than 4 args, pad record size to multiple of native memory */
    /*  transfer size.  Reserve space for extra args                     */
    for (i = 0; i < pads; i++)
    {
        *--saddr = 0;
    }

    ulong *sp = saddr;

    // *saddr is now pointing to the last pad word.

    // TODO: Initialize process context.

    /* Arguments passed in registers (r0-r3)	*/
    va_start(ap, nargs);	// init ap to point to the first arg
    for(i=0; i<4; i++){
    	if(nargs>=i){
    		ppcb->regs[i] = va_arg(ap, ulong);	// place arg in argument register
    	}else{
    		ppcb->regs[i] = 0;
    	}
    }

    /* Arguments passed via runtime stack	*/
    for(i=0; i<pads; i++){
    	*saddr = (ulong)va_arg(ap, ulong);	// push the rest of the args to the stack
    	saddr++;
    }
    va_end(ap);	// end var args

    /* Initialize process context	*/
    for(i=4; i<=11; i++){
    	ppcb->regs[i] = 0;
    }
    ppcb->regs[PREG_SP] = (ulong)sp;
    ppcb->regs[PREG_PC] = (ulong)funcaddr;
    ppcb->regs[PREG_LR] = (ulong)&userret;

    return pid;
}

/**
 * Obtain a new (free) process id.
 * @return a free process id, SYSERR if all ids are used
 */
static pid_typ newpid(void)
{
    pid_typ pid;                /* process id to return     */
    static pid_typ nextpid = 0;

    for (pid = 0; pid < NPROC; pid++)
    {                           /* check all NPROC slots    */
        nextpid = (nextpid + 1) % NPROC;
        if (PRFREE == proctab[nextpid].state)
        {
            return nextpid;
        }
    }
    return SYSERR;
}

/**
 * Entered when a process exits by return.
 */
void userret(void)
{
    kill(currpid);
}
