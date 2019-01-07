#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

//bs_map_t bsm_tab[];


SYSCALL release_bs(bsd_t bs_id) {
	bsm_tab[bs_id].bs_status = BSM_UNMAPPED;
  /* release the backing store with ID bs_id */
   	return OK;
}

