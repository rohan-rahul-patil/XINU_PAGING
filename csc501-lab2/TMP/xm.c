/* xm.c = xmmap xmunmap */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>


/*-------------------------------------------------------------------------
 * xmmap - xmmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmmap(int virtpage, bsd_t source, int npages)
{
	if(virtpage < 4096 || source > 7 || npages > 256)
		return SYSERR;
  	
 	int pid = getpid();
 	return(bsm_map(pid, virtpage, source, npages));
}



/*-------------------------------------------------------------------------
 * xmunmap - xmunmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmunmap(int virtpage)
{
	if(virtpage < 4096) 
		return SYSERR;
	int pid = getpid();
 	return(bsm_unmap(pid, virtpage, 0)); 
}
