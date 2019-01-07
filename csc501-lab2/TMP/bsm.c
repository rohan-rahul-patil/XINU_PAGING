/* bsm.c - manage the backing store mapping*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

//bs_map_t bsm_tab[];



/*-------------------------------------------------------------------------
 * init_bsm- initialize bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_bsm()
{
	int bs_id;
	for(bs_id = 0; bs_id < BSM_TAB_SIZE; bs_id++) { 
		bsm_tab[bs_id].bs_status = BSM_UNMAPPED;
		bsm_tab[bs_id].bs_pid = -1;          
  		bsm_tab[bs_id].bs_vpno = -1;        
  		bsm_tab[bs_id].bs_npages = 0;      
	}

	return(OK);
}

/*-------------------------------------------------------------------------
 * get_bsm - get a free entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL get_bsm(int* avail)
{
	int bs_id;
	for(bs_id = 0; bs_id < BSM_TAB_SIZE; bs_id++) {
                if(bsm_tab[bs_id].bs_status == BSM_UNMAPPED){
			*avail = bs_id;
			return(OK);
		}
	}

	return(SYSERR);
}	


/*-------------------------------------------------------------------------
 * free_bsm - free an entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL free_bsm(int i)
{
	if(0 <= i && i < BSM_TAB_SIZE) {
		bsm_tab[i].bs_status = BSM_UNMAPPED;
		bsm_tab[i].bs_pid = -1;  
                bsm_tab[i].bs_vpno = -1;   
                bsm_tab[i].bs_npages = 0;  
		return(OK);	
	}
	else
		return(SYSERR);

}

/*-------------------------------------------------------------------------
 * bsm_lookup - lookup bsm_tab and find the corresponding entry
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_lookup(int pid, long vaddr, int* store, int* pageth)
{
	int bs_id;
	for(bs_id = 0; bs_id < BSM_TAB_SIZE; bs_id++){
                if(bsm_tab[bs_id].bs_status == BSM_MAPPED && bsm_tab[bs_id].bs_pid == pid){
			if((int)(vaddr / NBPG) >= bsm_tab[bs_id].bs_vpno && (int)(vaddr / NBPG) < bsm_tab[bs_id].bs_vpno + bsm_tab[bs_id].bs_npages) {
				*store = bs_id;
				*pageth = (int)((vaddr/NBPG) - bsm_tab[bs_id].bs_vpno);
				return(OK);	
			}
		}
	}
	return(SYSERR);
}


/*-------------------------------------------------------------------------
 * bsm_map - add an mapping into bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_map(int pid, int vpno, int source, int npages)
{	
	if(0 <= source && source < BSM_TAB_SIZE) {
		bsm_tab[source].bs_status = BSM_MAPPED;
		bsm_tab[source].bs_pid = pid;
		bsm_tab[source].bs_vpno = vpno;
		bsm_tab[source].bs_npages = npages;
	}
	else 
		return(SYSERR);
}



/*-------------------------------------------------------------------------
 * bsm_unmap - delete an mapping from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_unmap(int pid, int vpno, int flag)
{
	int bs_id;
	for(bs_id = 0; bs_id < BSM_TAB_SIZE; bs_id++) {
		if(bsm_tab[bs_id].bs_pid == pid && bsm_tab[bs_id].bs_vpno == vpno) {
			bsm_tab[bs_id].bs_status = BSM_UNMAPPED;
			return(OK);
		}
	}
	
	return(SYSERR);
}


