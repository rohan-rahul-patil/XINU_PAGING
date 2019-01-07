/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <q.h>
#include <stdio.h>
#include <paging.h>

/*------------------------------------------------------------------------
 * kill  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
SYSCALL kill(int pid)
{

	STATWORD ps;    
	struct	pentry	*pptr;		/* points to proc. table for pid*/
	int	dev;

	disable(ps);

	int frame, store, pageth, vpno = MAXINT;
        unsigned long virtual_addr;
        char * physical_addr;
      
        for(frame = 0; frame < NFRAMES; frame++) {
                if(frm_tab[frame].fr_pid == pid) {

                        if(frm_tab[frame].fr_vpno < vpno) {
                                vpno = frm_tab[frame].fr_vpno;
                        }

                        virtual_addr = (frm_tab[frame].fr_vpno * NBPG);

                        if(bsm_lookup(pid, virtual_addr, &store, &pageth) != SYSERR) {
                        
				physical_addr = (char *)((FRAME0 + frame) * NBPG);
	                        write_bs(physical_addr, store, pageth);
			}
                    
                
                        free_frm(frame);
                }
        }


        bsm_unmap(pid, vpno, 0);
	bsm_unmap(pid, 4096, 0);
        
       	free_frm((proctab[pid].pdbr / NBPG) - FRAME0);
                
	
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate==PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	if (--numproc == 0)
		xdone();

	dev = pptr->pdevs[0];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->pdevs[1];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->ppagedev;
	if (! isbaddev(dev) )
		close(dev);
	
	send(pptr->pnxtkin, pid);

	freestk(pptr->pbase, pptr->pstklen);

	switch (pptr->pstate) {

	case PRCURR:	pptr->pstate = PRFREE;	/* suicide */
			resched();

	case PRWAIT:	semaph[pptr->psem].semcnt++;

	case PRREADY:	dequeue(pid);
			pptr->pstate = PRFREE;
			break;

	case PRSLEEP:
	case PRTRECV:	unsleep(pid);
						/* fall through	*/
	default:	pptr->pstate = PRFREE;
	}
	
	restore(ps);
	return(OK);
}
