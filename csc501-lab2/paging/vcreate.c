/* vcreate.c - vcreate */
    
#include <conf.h>
#include <i386.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <paging.h>

/*
static unsigned long esp;
*/

LOCAL	newpid();
/*------------------------------------------------------------------------
 *  create  -  create a process to start running a procedure
 *------------------------------------------------------------------------
 */
SYSCALL vcreate(procaddr,ssize,hsize,priority,name,nargs,args)
	int	*procaddr;		/* procedure address		*/
	int	ssize;			/* stack size in words		*/
	int	hsize;			/* virtual heap size in pages	*/
	int	priority;		/* process priority > 0		*/
	char	*name;			/* name (for debugging)		*/
	int	nargs;			/* number of args that follow	*/
	long	args;			/* arguments (treated like an	*/
					/* array in the code)		*/
{

	int pid;
	unsigned long b_address;
	
	pid = create(procaddr,ssize,priority,name,nargs,args);

	// heap allocation
	int block_no;
	if(get_bsm(&block_no) == SYSERR) {	
		return SYSERR;
	}
	
	if(bsm_map(pid, 4096 , block_no, hsize) == SYSERR) {
		kprintf("could not map backing store\n");
		return SYSERR;
	}
	

        proctab[pid].store = block_no;         
        proctab[pid].vhpno = bsm_tab[block_no].bs_vpno;       
        proctab[pid].vhpnpages = hsize;                     

	
	struct mblock *m;
	m = BACKING_STORE_BASE + block_no * BACKING_STORE_UNIT_SIZE;
	m->mlen = hsize * NBPG;
	m->mnext = NULL;
	proctab[pid].vmemlist->mnext = (4096 * NBPG);

	kprintf("process created\n");
	return(pid);
}

/*------------------------------------------------------------------------
 * newpid  --  obtain a new (free) process id
 *------------------------------------------------------------------------
 */
LOCAL	newpid()
{
	int	pid;			/* process id to return		*/
	int	i;

	for (i=0 ; i<NPROC ; i++) {	/* check all NPROC slots	*/
		if ( (pid=nextproc--) <= 0)
			nextproc = NPROC-1;
		if (proctab[pid].pstate == PRFREE)
			return(pid);
	}
	return(SYSERR);
}
