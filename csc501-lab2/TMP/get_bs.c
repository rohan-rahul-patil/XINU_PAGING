#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

//bs_map_t bsm_tab[];


int get_bs(bsd_t bs_id, unsigned int npages) {

  /* requests a new mapping of npages with ID map_id */
	
	if(bs_id >= BSM_TAB_SIZE || npages == 0 || npages > 256) 
		return(SYSERR);
	if(bsm_tab[bs_id].bs_status == BSM_MAPPED) 
		return 	bsm_tab[bs_id].bs_npages;
	else
    		return npages;
}


